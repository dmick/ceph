#include <iostream>
#include <fstream>
#include <cstdlib>

#include "include/rbd/librbd.hpp"
#include "include/rados/librados.hpp"
#include "common/errno.h"
#include "include/atomic.h"

using namespace std;

// wrap a system/library call with return-code checking (if you don't get
// 'expect', whine and exit

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

struct comparg {
	uint64_t offset;
	ceph::bufferlist *blp;
	librbd::RBD::AioCompletion *cp;
};

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


	{
		librbd::Image image;
		WRAP(R.open(ioctx, image, "image"), 0);
		// fill with random data
		ifstream urandom("/dev/urandom", ifstream::in);
		char *b = new char [SIZE];
		urandom.read(b, SIZE);
		urandom.close();
		ceph::bufferlist bl;
		bl.append(b, SIZE);
		WRAP(image.write(0, SIZE, bl), SIZE);
		delete [] b;
		// image goes out of scope, so, destroyed/closed
	}

	// reopen image for reading
	librbd::Image image;
	WRAP(R.open(ioctx, image, "image"), 0);
	WRAP(image.snap_create("snap"), 0);
	WRAP(image.snap_protect("snap"), 0);
	for (;;) {
		WRAP(R.clone(ioctx, "image", "snap", ioctx, "clone",
			     RBD_FEATURE_LAYERING, &order), 0);
		{
			librbd::Image clone;
			WRAP(R.open(ioctx, clone, "clone", 0), 0);
			// set up some aio_reads from the parent
			for (int i = 0; i < 2048; i++) {
				ceph::bufferlist *blp = new ceph::bufferlist(1);
				comparg *cap = new comparg;
				librbd::RBD::AioCompletion *cp = new
				    librbd::RBD::AioCompletion((void *)cap,
						 	       read_cb);
				cap->cp = cp;
				cap->offset = (((uint64_t)rand() << 32)
						| rand()) % SIZE;
				cap->blp = blp;
				inflight.inc();
				image.aio_read(cap->offset, 1, *(cap->blp), cp);
			}
			uint64_t before = inflight.read();
			WRAP(clone.flatten(), 0);
			cout << "before flatten: " << before;
			cout << " after: " << inflight.read() << endl;
		}
		WRAP(R.remove(ioctx, "clone"), 0);
	}
}

void
read_cb(librbd::completion_t cb, void *arg)
{
	comparg *cap = (comparg *)arg;
	inflight.dec();
	delete cap->blp;
	cap->cp->release();
	delete cap;
	// int ofs = (uint64_t)arg;
	// cout << "read_cb " << ofs << endl;
}

