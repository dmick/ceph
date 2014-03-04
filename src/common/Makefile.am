common_objs = \
	ceph_ver.o \
	common/DecayCounter.lo \
	common/LogClient.lo \
	common/LogEntry.lo \
	common/PrebufferedStreambuf.lo \
	common/SloppyCRCMap.lo \
	common/BackTrace.lo \
	common/perf_counters.lo \
	common/Mutex.lo \
	common/OutputDataSocket.lo \
	common/admin_socket.lo \
	common/admin_socket_client.lo \
	common/cmdparse.lo \
	common/escape.lo \
	common/Clock.lo \
	common/Throttle.lo \
	common/Timer.lo \
	common/Finisher.lo \
	common/environment.lo\
	common/assert.lo \
	common/run_cmd.lo \
	common/WorkQueue.lo \
	common/ConfUtils.lo \
	common/MemoryModel.lo \
	common/armor.lo \
	common/fd.lo \
	common/xattr.lo \
	common/safe_io.lo \
	common/snap_types.lo \
	common/str_list.lo \
	common/str_map.lo \
	common/errno.lo \
	common/RefCountedObj.lo \
	common/blkdev.lo \
	common/common_init.lo \
	common/pipe.lo \
	common/ceph_argparse.lo \
	common/ceph_context.lo \
	common/buffer.lo \
	common/code_environment.lo \
	common/dout.lo \
	common/signal.lo \
	common/simple_spin.lo \
	common/Thread.lo \
	common/Formatter.lo \
	common/HeartbeatMap.lo \
	common/config.lo \
	common/utf8.lo \
	common/mime.lo \
	common/strtol.lo \
	common/page.lo \
	common/lockdep.lo \
	common/version.lo \
	common/hex.lo \
	common/entity_name.lo \
	common/ceph_crypto.lo \
	common/ceph_crypto_cms.lo \
	common/ceph_json.lo \
	common/ipaddr.lo \
	common/pick_address.lo \
	common/util.lo \
	common/TextTable.lo \
	common/ceph_fs.lo \
	common/ceph_hash.lo \
	common/ceph_strings.lo \
	common/ceph_frag.lo \
	common/addr_parsing.lo \
	common/hobject.lo \
	common/bloom_filter.lo \
	common/linux_version.lo

if LINUX
common_objs += \
	common/secret.lo
endif

# these should go out of libcommon
common_objs += \
	mon/MonCap.lo \
	mon/MonClient.lo \
	mon/MonMap.lo \
	osd/OSDMap.lo \
	osd/osd_types.lo \
	osd/HitSet.lo \
	mds/MDSMap.lo \
	mds/inode_backtrace.lo \
	mds/mdstypes.lo 

# inject crc in common
libcommon_crc_la_SOURCES = \
	common/sctp_crc32.c \
	common/crc32c.cc \
	common/crc32c_intel_baseline.c \
	common/crc32c_intel_fast.c

if WITH_GOOD_YASM_ELF64
libcommon_crc_la_SOURCES += common/crc32c_intel_fast_asm.S common/crc32c_intel_fast_zero_asm.S
libcommon_crc_la_LIBTOOLFLAGS = --tag=CC
endif
LIBCOMMON_DEPS += libcommon_crc.la
noinst_LTLIBRARIES += libcommon_crc.la

noinst_HEADERS += \
	common/bloom_filter.hpp \
	common/sctp_crc32.h \
	common/crc32c_intel_baseline.h \
	common/crc32c_intel_fast.h


# important; libmsg before libauth!
LIBCOMMON_DEPS += \
	$(LIBMSG) $(LIBAUTH) \
	$(LIBCRUSH) $(LIBJSON_SPIRIT) $(LIBLOG) $(LIBARCH) \
	$(KEYUTILS_LIB)

if LINUX
LIBCOMMON_DEPS += -lrt
endif # LINUX

libcommon_la_LIBADD = $(common_objs)

noinst_HEADERS += \
	common/BackTrace.h \
	common/RefCountedObj.h \
	common/HeartbeatMap.h \
	common/LogClient.h \
	common/LogEntry.h \
	common/Preforker.h \
	common/SloppyCRCMap.h \
	common/WorkQueue.h \
	common/PrioritizedQueue.h \
	common/ceph_argparse.h \
	common/ceph_context.h \
	common/xattr.h \
	common/blkdev.h \
	common/compiler_extensions.h \
	common/debug.h \
	common/dout.h \
	common/escape.h \
	common/fd.h \
	common/version.h \
	common/hex.h \
	common/entity_name.h \
	common/errno.h \
	common/environment.h \
	common/likely.h \
	common/lockdep.h \
	common/obj_bencher.h \
	common/snap_types.h \
	common/Clock.h \
	common/Cond.h \
	common/ConfUtils.h \
	common/DecayCounter.h \
	common/Finisher.h \
	common/Formatter.h \
	common/perf_counters.h \
	common/OutputDataSocket.h \
	common/admin_socket.h \
	common/admin_socket_client.h \
	common/shared_cache.hpp \
	common/tracked_int_ptr.hpp \
	common/simple_cache.hpp \
	common/sharedptr_registry.hpp \
	common/map_cacher.hpp \
	common/MemoryModel.h \
	common/Mutex.h \
	common/PrebufferedStreambuf.h \
	common/RWLock.h \
	common/Semaphore.h \
	common/SimpleRNG.h \
	common/TextTable.h \
	common/Thread.h \
	common/Throttle.h \
	common/Timer.h \
	common/TrackedOp.h \
	common/arch.h \
	common/armor.h \
	common/common_init.h \
	common/pipe.h \
	common/code_environment.h \
	common/signal.h \
	common/simple_spin.h \
	common/run_cmd.h \
	common/safe_io.h \
	common/config.h \
	common/config_obs.h \
	common/config_opts.h \
	common/ceph_crypto.h \
	common/ceph_crypto_cms.h \
	common/ceph_json.h \
	common/lru_map.h \
	common/utf8.h \
	common/mime.h \
	common/pick_address.h \
	common/secret.h \
	common/strtol.h \
	common/static_assert.h \
	common/AsyncReserver.h \
	common/sync_filesystem.h \
	common/cmdparse.h \
	common/hobject.h \
	common/linux_version.h
