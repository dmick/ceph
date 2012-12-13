#include <iostream>
#include <fstream>
#include <cstdlib>

#include "include/rbd/librbd.hpp"
#include "include/rados/librados.hpp"
#include "common/errno.h"
#include "include/atomic.h"

using namespace std;

#define WRAP(call, expect)				\
	{					\
	int r;					\
	if ((r = call) != expect) {			\
		cerr << #call << " failed: " << cpp_strerror(r) << endl; \
		exit(1); 			\
	}					\
	}

void read_cb(librbd::completion_t cb, void *arg);
atomic_t inflight;

int
main()
{
	// open the cluster
	librados::Rados rados;

	WRAP(rados.init("flatcache"), 0);
	WRAP(rados.conf_read_file("ceph.conf"), 0);

	WRAP(rados.connect(), 0);

	librados::IoCtx ioctx;
	WRAP(rados.ioctx_create("rbd", ioctx), 0);

	// create an image
#define MEG	1024ULL * 1024ULL
#define SIZE	(4 * MEG)
	librbd::RBD R;
	int order = 22;
  	WRAP(R.create2(ioctx, "image", SIZE, RBD_FEATURE_LAYERING, &order), 0);

	librbd::Image image;
	WRAP(R.open(ioctx, image, "image"), 0);

	{
		// fill with random data
		ifstream urandom("/dev/urandom", ifstream::in);
		char *b = new char [SIZE];
		urandom.read(b, SIZE);
		urandom.close();
		ceph::bufferlist bl;
		bl.append(b, SIZE);
		WRAP(image.write(0, SIZE, bl), SIZE);
	}

	WRAP(image.snap_create("snap"), 0);
	WRAP(image.snap_protect("snap"), 0);
	ceph::bufferlist bl(1);
	for (;;) {
		cout << "clone" << endl;
		WRAP(R.clone(ioctx, "image", "snap", ioctx, "clone",
			     RBD_FEATURE_LAYERING, &order), 0);
		{
			librbd::Image clone;
			WRAP(R.open(ioctx, clone, "clone", 0), 0);
			// set up some aio_reads from the parent
			for (int i = 0; i < 10000; i++) {
				uint64_t ofs = (((uint64_t)rand() << 32)
						| rand()) % SIZE;
				librbd::RBD::AioCompletion
				    c((void *)ofs, read_cb);
				// yes, all into same bl
				inflight.inc();
				image.aio_read(ofs, 1, bl, &c);
			}
			cout << "flatten, inflight: " << inflight.read() << endl;
			WRAP(clone.flatten(), 0);
			cout << "flatten done, inflight: " << inflight.read() << endl;
		}
		cout << "remove" << endl;
		WRAP(R.remove(ioctx, "clone"), 0);
	}
}

void
read_cb(librbd::completion_t cb, void *arg)
{
	inflight.dec();
	// int ofs = (uint64_t)arg;
	// cout << "read_cb " << ofs << endl;
}

