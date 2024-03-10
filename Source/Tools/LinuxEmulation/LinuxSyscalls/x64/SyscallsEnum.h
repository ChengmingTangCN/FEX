// SPDX-License-Identifier: MIT
/*
$info$
tags: LinuxSyscalls|syscalls-x86-64
$end_info$
*/
#pragma once

namespace FEX::HLE::x64 {
///< Enum containing all x86-64 linux syscalls for the guest kernel version
enum Syscalls_x64 {
  SYSCALL_x64_read = 0,
  SYSCALL_x64_write = 1,
  SYSCALL_x64_open = 2,
  SYSCALL_x64_close = 3,
  SYSCALL_x64_stat = 4,
  SYSCALL_x64_fstat = 5,
  SYSCALL_x64_lstat = 6,
  SYSCALL_x64_poll = 7,
  SYSCALL_x64_lseek = 8,
  SYSCALL_x64_mmap = 9,
  SYSCALL_x64_mprotect = 10,
  SYSCALL_x64_munmap = 11,
  SYSCALL_x64_brk = 12,
  SYSCALL_x64_rt_sigaction = 13,
  SYSCALL_x64_rt_sigprocmask = 14,
  SYSCALL_x64_rt_sigreturn = 15,
  SYSCALL_x64_ioctl = 16,
  SYSCALL_x64_pread_64 = 17,
  SYSCALL_x64_pwrite_64 = 18,
  SYSCALL_x64_readv = 19,
  SYSCALL_x64_writev = 20,
  SYSCALL_x64_access = 21,
  SYSCALL_x64_pipe = 22,
  SYSCALL_x64_select = 23,
  SYSCALL_x64_sched_yield = 24,
  SYSCALL_x64_mremap = 25,
  SYSCALL_x64_msync = 26,
  SYSCALL_x64_mincore = 27,
  SYSCALL_x64_madvise = 28,
  SYSCALL_x64__shmget = 29,
  SYSCALL_x64__shmat = 30,
  SYSCALL_x64__shmctl = 31,
  SYSCALL_x64_dup = 32,
  SYSCALL_x64_dup2 = 33,
  SYSCALL_x64_pause = 34,
  SYSCALL_x64_nanosleep = 35,
  SYSCALL_x64_getitimer = 36,
  SYSCALL_x64_alarm = 37,
  SYSCALL_x64_setitimer = 38,
  SYSCALL_x64_getpid = 39,
  SYSCALL_x64_sendfile = 40,
  SYSCALL_x64_socket = 41,
  SYSCALL_x64_connect = 42,
  SYSCALL_x64_accept = 43,
  SYSCALL_x64_sendto = 44,
  SYSCALL_x64_recvfrom = 45,
  SYSCALL_x64_sendmsg = 46,
  SYSCALL_x64_recvmsg = 47,
  SYSCALL_x64_shutdown = 48,
  SYSCALL_x64_bind = 49,
  SYSCALL_x64_listen = 50,
  SYSCALL_x64_getsockname = 51,
  SYSCALL_x64_getpeername = 52,
  SYSCALL_x64_socketpair = 53,
  SYSCALL_x64_setsockopt = 54,
  SYSCALL_x64_getsockopt = 55,
  SYSCALL_x64_clone = 56,
  SYSCALL_x64_fork = 57,
  SYSCALL_x64_vfork = 58,
  SYSCALL_x64_execve = 59,
  SYSCALL_x64_exit = 60,
  SYSCALL_x64_wait4 = 61,
  SYSCALL_x64_kill = 62,
  SYSCALL_x64_uname = 63,
  SYSCALL_x64_semget = 64,
  SYSCALL_x64_semop = 65,
  SYSCALL_x64_semctl = 66,
  SYSCALL_x64__shmdt = 67,
  SYSCALL_x64_msgget = 68,
  SYSCALL_x64_msgsnd = 69,
  SYSCALL_x64_msgrcv = 70,
  SYSCALL_x64_msgctl = 71,
  SYSCALL_x64_fcntl = 72,
  SYSCALL_x64_flock = 73,
  SYSCALL_x64_fsync = 74,
  SYSCALL_x64_fdatasync = 75,
  SYSCALL_x64_truncate = 76,
  SYSCALL_x64_ftruncate = 77,
  SYSCALL_x64_getdents = 78,
  SYSCALL_x64_getcwd = 79,
  SYSCALL_x64_chdir = 80,
  SYSCALL_x64_fchdir = 81,
  SYSCALL_x64_rename = 82,
  SYSCALL_x64_mkdir = 83,
  SYSCALL_x64_rmdir = 84,
  SYSCALL_x64_creat = 85,
  SYSCALL_x64_link = 86,
  SYSCALL_x64_unlink = 87,
  SYSCALL_x64_symlink = 88,
  SYSCALL_x64_readlink = 89,
  SYSCALL_x64_chmod = 90,
  SYSCALL_x64_fchmod = 91,
  SYSCALL_x64_chown = 92,
  SYSCALL_x64_fchown = 93,
  SYSCALL_x64_lchown = 94,
  SYSCALL_x64_umask = 95,
  SYSCALL_x64_gettimeofday = 96,
  SYSCALL_x64_getrlimit = 97,
  SYSCALL_x64_getrusage = 98,
  SYSCALL_x64_sysinfo = 99,
  SYSCALL_x64_times = 100,
  SYSCALL_x64_ptrace = 101,
  SYSCALL_x64_getuid = 102,
  SYSCALL_x64_syslog = 103,
  SYSCALL_x64_getgid = 104,
  SYSCALL_x64_setuid = 105,
  SYSCALL_x64_setgid = 106,
  SYSCALL_x64_geteuid = 107,
  SYSCALL_x64_getegid = 108,
  SYSCALL_x64_setpgid = 109,
  SYSCALL_x64_getppid = 110,
  SYSCALL_x64_getpgrp = 111,
  SYSCALL_x64_setsid = 112,
  SYSCALL_x64_setreuid = 113,
  SYSCALL_x64_setregid = 114,
  SYSCALL_x64_getgroups = 115,
  SYSCALL_x64_setgroups = 116,
  SYSCALL_x64_setresuid = 117,
  SYSCALL_x64_getresuid = 118,
  SYSCALL_x64_setresgid = 119,
  SYSCALL_x64_getresgid = 120,
  SYSCALL_x64_getpgid = 121,
  SYSCALL_x64_setfsuid = 122,
  SYSCALL_x64_setfsgid = 123,
  SYSCALL_x64_getsid = 124,
  SYSCALL_x64_capget = 125,
  SYSCALL_x64_capset = 126,
  SYSCALL_x64_rt_sigpending = 127,
  SYSCALL_x64_rt_sigtimedwait = 128,
  SYSCALL_x64_rt_sigqueueinfo = 129,
  SYSCALL_x64_rt_sigsuspend = 130,
  SYSCALL_x64_sigaltstack = 131,
  SYSCALL_x64_utime = 132,
  SYSCALL_x64_mknod = 133,
  // No entrypoint. -ENOSYS
  SYSCALL_x64_uselib = 134,
  SYSCALL_x64_personality = 135,
  SYSCALL_x64_ustat = 136,
  SYSCALL_x64_statfs = 137,
  SYSCALL_x64_fstatfs = 138,
  SYSCALL_x64_sysfs = 139,
  SYSCALL_x64_getpriority = 140,
  SYSCALL_x64_setpriority = 141,
  SYSCALL_x64_sched_setparam = 142,
  SYSCALL_x64_sched_getparam = 143,
  SYSCALL_x64_sched_setscheduler = 144,
  SYSCALL_x64_sched_getscheduler = 145,
  SYSCALL_x64_sched_get_priority_max = 146,
  SYSCALL_x64_sched_get_priority_min = 147,
  SYSCALL_x64_sched_rr_get_interval = 148,
  SYSCALL_x64_mlock = 149,
  SYSCALL_x64_munlock = 150,
  SYSCALL_x64_mlockall = 151,
  SYSCALL_x64_munlockall = 152,
  SYSCALL_x64_vhangup = 153,
  SYSCALL_x64_modify_ldt = 154,
  SYSCALL_x64_pivot_root = 155,
  SYSCALL_x64__sysctl = 156,
  SYSCALL_x64_prctl = 157,
  SYSCALL_x64_arch_prctl = 158,
  SYSCALL_x64_adjtimex = 159,
  SYSCALL_x64_setrlimit = 160,
  SYSCALL_x64_chroot = 161,
  SYSCALL_x64_sync = 162,
  SYSCALL_x64_acct = 163,
  SYSCALL_x64_settimeofday = 164,
  SYSCALL_x64_mount = 165,
  SYSCALL_x64_umount2 = 166,
  SYSCALL_x64_swapon = 167,
  SYSCALL_x64_swapoff = 168,
  SYSCALL_x64_reboot = 169,
  SYSCALL_x64_sethostname = 170,
  SYSCALL_x64_setdomainname = 171,
  SYSCALL_x64_iopl = 172,
  SYSCALL_x64_ioperm = 173,
  // No entrypoint. -ENOSYS
  SYSCALL_x64_create_module = 174,
  SYSCALL_x64_init_module = 175,
  SYSCALL_x64_delete_module = 176,
  // No entrypoint. -ENOSYS
  SYSCALL_x64_get_kernel_syms = 177,
  // No entrypoint. -ENOSYS
  SYSCALL_x64_query_module = 178,
  SYSCALL_x64_quotactl = 179,
  // No entrypoint. -ENOSYS
  SYSCALL_x64_nfsservctl = 180,
  // No entrypoint. -ENOSYS
  SYSCALL_x64_getpmsg = 181,
  // No entrypoint. -ENOSYS
  SYSCALL_x64_putpmsg = 182,
  // No entrypoint. -ENOSYS
  SYSCALL_x64_afs_syscall = 183,
  // No entrypoint. -ENOSYS
  SYSCALL_x64_tuxcall = 184,
  // No entrypoint. -ENOSYS
  SYSCALL_x64_security = 185,
  SYSCALL_x64_gettid = 186,
  SYSCALL_x64_readahead = 187,
  SYSCALL_x64_setxattr = 188,
  SYSCALL_x64_lsetxattr = 189,
  SYSCALL_x64_fsetxattr = 190,
  SYSCALL_x64_getxattr = 191,
  SYSCALL_x64_lgetxattr = 192,
  SYSCALL_x64_fgetxattr = 193,
  SYSCALL_x64_listxattr = 194,
  SYSCALL_x64_llistxattr = 195,
  SYSCALL_x64_flistxattr = 196,
  SYSCALL_x64_removexattr = 197,
  SYSCALL_x64_lremovexattr = 198,
  SYSCALL_x64_fremovexattr = 199,
  SYSCALL_x64_tkill = 200,
  SYSCALL_x64_time = 201,
  SYSCALL_x64_futex = 202,
  SYSCALL_x64_sched_setaffinity = 203,
  SYSCALL_x64_sched_getaffinity = 204,
  // No entrypoint. -ENOSYS
  SYSCALL_x64_set_thread_area = 205,
  SYSCALL_x64_io_setup = 206,
  SYSCALL_x64_io_destroy = 207,
  SYSCALL_x64_io_getevents = 208,
  SYSCALL_x64_io_submit = 209,
  SYSCALL_x64_io_cancel = 210,
  // No entrypoint. -ENOSYS
  SYSCALL_x64_get_thread_area = 211,
  // No entrypoint. -ENOSYS
  SYSCALL_x64_lookup_dcookie = 212,
  SYSCALL_x64_epoll_create = 213,
  // No entrypoint. -ENOSYS
  SYSCALL_x64_epoll_ctl_old = 214,
  // No entrypoint. -ENOSYS
  SYSCALL_x64_epoll_wait_old = 215,
  SYSCALL_x64_remap_file_pages = 216,
  SYSCALL_x64_getdents64 = 217,
  SYSCALL_x64_set_tid_address = 218,
  SYSCALL_x64_restart_syscall = 219,
  SYSCALL_x64_semtimedop = 220,
  SYSCALL_x64_fadvise64 = 221,
  SYSCALL_x64_timer_create = 222,
  SYSCALL_x64_timer_settime = 223,
  SYSCALL_x64_timer_gettime = 224,
  SYSCALL_x64_timer_getoverrun = 225,
  SYSCALL_x64_timer_delete = 226,
  SYSCALL_x64_clock_settime = 227,
  SYSCALL_x64_clock_gettime = 228,
  SYSCALL_x64_clock_getres = 229,
  SYSCALL_x64_clock_nanosleep = 230,
  SYSCALL_x64_exit_group = 231,
  SYSCALL_x64_epoll_wait = 232,
  SYSCALL_x64_epoll_ctl = 233,
  SYSCALL_x64_tgkill = 234,
  SYSCALL_x64_utimes = 235,
  // No entrypoint. -ENOSYS
  SYSCALL_x64_vserver = 236,
  SYSCALL_x64_mbind = 237,
  SYSCALL_x64_set_mempolicy = 238,
  SYSCALL_x64_get_mempolicy = 239,
  SYSCALL_x64_mq_open = 240,
  SYSCALL_x64_mq_unlink = 241,
  SYSCALL_x64_mq_timedsend = 242,
  SYSCALL_x64_mq_timedreceive = 243,
  SYSCALL_x64_mq_notify = 244,
  SYSCALL_x64_mq_getsetattr = 245,
  SYSCALL_x64_kexec_load = 246,
  SYSCALL_x64_waitid = 247,
  SYSCALL_x64_add_key = 248,
  SYSCALL_x64_request_key = 249,
  SYSCALL_x64_keyctl = 250,
  SYSCALL_x64_ioprio_set = 251,
  SYSCALL_x64_ioprio_get = 252,
  SYSCALL_x64_inotify_init = 253,
  SYSCALL_x64_inotify_add_watch = 254,
  SYSCALL_x64_inotify_rm_watch = 255,
  SYSCALL_x64_migrate_pages = 256,
  SYSCALL_x64_openat = 257,
  SYSCALL_x64_mkdirat = 258,
  SYSCALL_x64_mknodat = 259,
  SYSCALL_x64_fchownat = 260,
  SYSCALL_x64_futimesat = 261,
  SYSCALL_x64_newfstatat = 262,
  SYSCALL_x64_unlinkat = 263,
  SYSCALL_x64_renameat = 264,
  SYSCALL_x64_linkat = 265,
  SYSCALL_x64_symlinkat = 266,
  SYSCALL_x64_readlinkat = 267,
  SYSCALL_x64_fchmodat = 268,
  SYSCALL_x64_faccessat = 269,
  SYSCALL_x64_pselect6 = 270,
  SYSCALL_x64_ppoll = 271,
  SYSCALL_x64_unshare = 272,
  SYSCALL_x64_set_robust_list = 273,
  SYSCALL_x64_get_robust_list = 274,
  SYSCALL_x64_splice = 275,
  SYSCALL_x64_tee = 276,
  SYSCALL_x64_sync_file_range = 277,
  SYSCALL_x64_vmsplice = 278,
  SYSCALL_x64_move_pages = 279,
  SYSCALL_x64_utimensat = 280,
  SYSCALL_x64_epoll_pwait = 281,
  SYSCALL_x64_signalfd = 282,
  SYSCALL_x64_timerfd_create = 283,
  SYSCALL_x64_eventfd = 284,
  SYSCALL_x64_fallocate = 285,
  SYSCALL_x64_timerfd_settime = 286,
  SYSCALL_x64_timerfd_gettime = 287,
  SYSCALL_x64_accept4 = 288,
  SYSCALL_x64_signalfd4 = 289,
  SYSCALL_x64_eventfd2 = 290,
  SYSCALL_x64_epoll_create1 = 291,
  SYSCALL_x64_dup3 = 292,
  SYSCALL_x64_pipe2 = 293,
  SYSCALL_x64_inotify_init1 = 294,
  SYSCALL_x64_preadv = 295,
  SYSCALL_x64_pwritev = 296,
  SYSCALL_x64_rt_tgsigqueueinfo = 297,
  SYSCALL_x64_perf_event_open = 298,
  SYSCALL_x64_recvmmsg = 299,
  SYSCALL_x64_fanotify_init = 300,
  SYSCALL_x64_fanotify_mark = 301,
  SYSCALL_x64_prlimit_64 = 302,
  SYSCALL_x64_name_to_handle_at = 303,
  SYSCALL_x64_open_by_handle_at = 304,
  SYSCALL_x64_clock_adjtime = 305,
  SYSCALL_x64_syncfs = 306,
  SYSCALL_x64_sendmmsg = 307,
  SYSCALL_x64_setns = 308,
  SYSCALL_x64_getcpu = 309,
  SYSCALL_x64_process_vm_readv = 310,
  SYSCALL_x64_process_vm_writev = 311,
  SYSCALL_x64_kcmp = 312,
  SYSCALL_x64_finit_module = 313,
  SYSCALL_x64_sched_setattr = 314,
  SYSCALL_x64_sched_getattr = 315,
  SYSCALL_x64_renameat2 = 316,
  SYSCALL_x64_seccomp = 317,
  SYSCALL_x64_getrandom = 318,
  SYSCALL_x64_memfd_create = 319,
  SYSCALL_x64_kexec_file_load = 320,
  SYSCALL_x64_bpf = 321,
  SYSCALL_x64_execveat = 322,
  SYSCALL_x64_userfaultfd = 323,
  SYSCALL_x64_membarrier = 324,
  SYSCALL_x64_mlock2 = 325,
  SYSCALL_x64_copy_file_range = 326,
  SYSCALL_x64_preadv2 = 327,
  SYSCALL_x64_pwritev2 = 328,
  SYSCALL_x64_pkey_mprotect = 329,
  SYSCALL_x64_pkey_alloc = 330,
  SYSCALL_x64_pkey_free = 331,
  SYSCALL_x64_statx = 332,
  SYSCALL_x64_io_pgetevents = 333,
  SYSCALL_x64_rseq = 334,
  SYSCALL_x64_pidfd_send_signal = 424,
  SYSCALL_x64_io_uring_setup = 425,
  SYSCALL_x64_io_uring_enter = 426,
  SYSCALL_x64_io_uring_register = 427,
  SYSCALL_x64_open_tree = 428,
  SYSCALL_x64_move_mount = 429,
  SYSCALL_x64_fsopen = 430,
  SYSCALL_x64_fsconfig = 431,
  SYSCALL_x64_fsmount = 432,
  SYSCALL_x64_fspick = 433,
  SYSCALL_x64_pidfd_open = 434,
  SYSCALL_x64_clone3 = 435,
  SYSCALL_x64_close_range = 436,
  SYSCALL_x64_openat2 = 437,
  SYSCALL_x64_pidfd_getfd = 438,
  SYSCALL_x64_faccessat2 = 439,
  SYSCALL_x64_process_madvise = 440,
  SYSCALL_x64_epoll_pwait2 = 441,
  SYSCALL_x64_mount_setattr = 442,
  SYSCALL_x64_quotactl_fd = 443,
  SYSCALL_x64_landlock_create_ruleset = 444,
  SYSCALL_x64_landlock_add_rule = 445,
  SYSCALL_x64_landlock_restrict_self = 446,
  SYSCALL_x64_memfd_secret = 447,
  SYSCALL_x64_process_mrelease = 448,
  SYSCALL_x64_futex_waitv = 449,
  SYSCALL_x64_set_mempolicy_home_node = 450,
  SYSCALL_x64_cachestat = 451,
  SYSCALL_x64_fchmodat2 = 452,
  SYSCALL_x64_map_shadow_stack = 453,
  SYSCALL_x64_futex_wake = 454,
  SYSCALL_x64_futex_wait = 455,
  SYSCALL_x64_futex_requeue = 456,
  SYSCALL_x64_statmount = 457,
  SYSCALL_x64_listmount = 458,
  SYSCALL_x64_lsm_get_self_attr = 459,
  SYSCALL_x64_lsm_set_self_attr = 460,
  SYSCALL_x64_lsm_list_modules = 461,
  SYSCALL_x64_MAX = 512,

  // Unsupported syscalls on this host
  SYSCALL_x64_waitpid = ~0,
  SYSCALL_x64_break = ~0,
  SYSCALL_x64_oldstat = ~0,
  SYSCALL_x64_umount = ~0,
  SYSCALL_x64_stime = ~0,
  SYSCALL_x64_oldfstat = ~0,
  SYSCALL_x64_stty = ~0,
  SYSCALL_x64_gtty = ~0,
  SYSCALL_x64_nice = ~0,
  SYSCALL_x64_ftime = ~0,
  SYSCALL_x64_prof = ~0,
  SYSCALL_x64_signal = ~0,
  SYSCALL_x64_lock = ~0,
  SYSCALL_x64_mpx = ~0,
  SYSCALL_x64_ulimit = ~0,
  SYSCALL_x64_oldolduname = ~0,
  SYSCALL_x64_sigaction = ~0,
  SYSCALL_x64_sgetmask = ~0,
  SYSCALL_x64_ssetmask = ~0,
  SYSCALL_x64_sigsuspend = ~0,
  SYSCALL_x64_sigpending = ~0,
  SYSCALL_x64_oldlstat = ~0,
  SYSCALL_x64_readdir = ~0,
  SYSCALL_x64_profil = ~0,
  SYSCALL_x64_socketcall = ~0,
  SYSCALL_x64_olduname = ~0,
  SYSCALL_x64_idle = ~0,
  SYSCALL_x64_vm86old = ~0,
  SYSCALL_x64_ipc = ~0,
  SYSCALL_x64_sigreturn = ~0,
  SYSCALL_x64_sigprocmask = ~0,
  SYSCALL_x64_bdflush = ~0,
  SYSCALL_x64__llseek = ~0,
  SYSCALL_x64__newselect = ~0,
  SYSCALL_x64_vm86 = ~0,
  SYSCALL_x64_ugetrlimit = ~0,
  SYSCALL_x64_mmap2 = ~0,
  SYSCALL_x64_truncate64 = ~0,
  SYSCALL_x64_ftruncate64 = ~0,
  SYSCALL_x64_stat64 = ~0,
  SYSCALL_x64_lstat64 = ~0,
  SYSCALL_x64_fstat64 = ~0,
  SYSCALL_x64_lchown32 = ~0,
  SYSCALL_x64_getuid32 = ~0,
  SYSCALL_x64_getgid32 = ~0,
  SYSCALL_x64_geteuid32 = ~0,
  SYSCALL_x64_getegid32 = ~0,
  SYSCALL_x64_setreuid32 = ~0,
  SYSCALL_x64_setregid32 = ~0,
  SYSCALL_x64_getgroups32 = ~0,
  SYSCALL_x64_setgroups32 = ~0,
  SYSCALL_x64_fchown32 = ~0,
  SYSCALL_x64_setresuid32 = ~0,
  SYSCALL_x64_getresuid32 = ~0,
  SYSCALL_x64_setresgid32 = ~0,
  SYSCALL_x64_getresgid32 = ~0,
  SYSCALL_x64_chown32 = ~0,
  SYSCALL_x64_setuid32 = ~0,
  SYSCALL_x64_setgid32 = ~0,
  SYSCALL_x64_setfsuid32 = ~0,
  SYSCALL_x64_setfsgid32 = ~0,
  SYSCALL_x64_fcntl64 = ~0,
  SYSCALL_x64_sendfile64 = ~0,
  SYSCALL_x64_statfs64 = ~0,
  SYSCALL_x64_fstatfs64 = ~0,
  SYSCALL_x64_fadvise64_64 = ~0,
  SYSCALL_x64_fstatat_64 = ~0,
  SYSCALL_x64_clock_gettime64 = ~0,
  SYSCALL_x64_clock_settime64 = ~0,
  SYSCALL_x64_clock_adjtime64 = ~0,
  SYSCALL_x64_clock_getres_time64 = ~0,
  SYSCALL_x64_clock_nanosleep_time64 = ~0,
  SYSCALL_x64_timer_gettime64 = ~0,
  SYSCALL_x64_timer_settime64 = ~0,
  SYSCALL_x64_timerfd_gettime64 = ~0,
  SYSCALL_x64_timerfd_settime64 = ~0,
  SYSCALL_x64_utimensat_time64 = ~0,
  SYSCALL_x64_pselect6_time64 = ~0,
  SYSCALL_x64_ppoll_time64 = ~0,
  SYSCALL_x64_io_pgetevents_time64 = ~0,
  SYSCALL_x64_recvmmsg_time64 = ~0,
  SYSCALL_x64_mq_timedsend_time64 = ~0,
  SYSCALL_x64_mq_timedreceive_time64 = ~0,
  SYSCALL_x64_semtimedop_time64 = ~0,
  SYSCALL_x64_rt_sigtimedwait_time64 = ~0,
  SYSCALL_x64_futex_time64 = ~0,
  SYSCALL_x64_sched_rr_get_interval_time64 = ~0,
};
}
