begin_unit
begin_include
include|#
directive|include
file|<winsock2.h>
end_include
begin_include
include|#
directive|include
file|<ws2tcpip.h>
end_include
begin_comment
comment|/*  * things that are not available in header files  */
end_comment
begin_typedef
DECL|typedef|pid_t
typedef|typedef
name|int
name|pid_t
typedef|;
end_typedef
begin_typedef
DECL|typedef|uid_t
typedef|typedef
name|int
name|uid_t
typedef|;
end_typedef
begin_typedef
DECL|typedef|socklen_t
typedef|typedef
name|int
name|socklen_t
typedef|;
end_typedef
begin_define
DECL|macro|hstrerror
define|#
directive|define
name|hstrerror
value|strerror
end_define
begin_define
DECL|macro|S_IFLNK
define|#
directive|define
name|S_IFLNK
value|0120000
end_define
begin_comment
DECL|macro|S_IFLNK
comment|/* Symbolic link */
end_comment
begin_define
DECL|macro|S_ISLNK
define|#
directive|define
name|S_ISLNK
parameter_list|(
name|x
parameter_list|)
value|(((x)& S_IFMT) == S_IFLNK)
end_define
begin_define
DECL|macro|S_ISSOCK
define|#
directive|define
name|S_ISSOCK
parameter_list|(
name|x
parameter_list|)
value|0
end_define
begin_define
DECL|macro|S_IRGRP
define|#
directive|define
name|S_IRGRP
value|0
end_define
begin_define
DECL|macro|S_IWGRP
define|#
directive|define
name|S_IWGRP
value|0
end_define
begin_define
DECL|macro|S_IXGRP
define|#
directive|define
name|S_IXGRP
value|0
end_define
begin_define
DECL|macro|S_IRWXG
define|#
directive|define
name|S_IRWXG
value|(S_IRGRP | S_IWGRP | S_IXGRP)
end_define
begin_define
DECL|macro|S_IROTH
define|#
directive|define
name|S_IROTH
value|0
end_define
begin_define
DECL|macro|S_IWOTH
define|#
directive|define
name|S_IWOTH
value|0
end_define
begin_define
DECL|macro|S_IXOTH
define|#
directive|define
name|S_IXOTH
value|0
end_define
begin_define
DECL|macro|S_IRWXO
define|#
directive|define
name|S_IRWXO
value|(S_IROTH | S_IWOTH | S_IXOTH)
end_define
begin_define
DECL|macro|S_ISUID
define|#
directive|define
name|S_ISUID
value|0
end_define
begin_define
DECL|macro|S_ISGID
define|#
directive|define
name|S_ISGID
value|0
end_define
begin_define
DECL|macro|S_ISVTX
define|#
directive|define
name|S_ISVTX
value|0
end_define
begin_define
DECL|macro|WIFEXITED
define|#
directive|define
name|WIFEXITED
parameter_list|(
name|x
parameter_list|)
value|1
end_define
begin_define
DECL|macro|WIFSIGNALED
define|#
directive|define
name|WIFSIGNALED
parameter_list|(
name|x
parameter_list|)
value|0
end_define
begin_define
DECL|macro|WEXITSTATUS
define|#
directive|define
name|WEXITSTATUS
parameter_list|(
name|x
parameter_list|)
value|((x)& 0xff)
end_define
begin_define
DECL|macro|WTERMSIG
define|#
directive|define
name|WTERMSIG
parameter_list|(
name|x
parameter_list|)
value|SIGTERM
end_define
begin_define
DECL|macro|EWOULDBLOCK
define|#
directive|define
name|EWOULDBLOCK
value|EAGAIN
end_define
begin_define
DECL|macro|SHUT_WR
define|#
directive|define
name|SHUT_WR
value|SD_SEND
end_define
begin_define
DECL|macro|SIGHUP
define|#
directive|define
name|SIGHUP
value|1
end_define
begin_define
DECL|macro|SIGQUIT
define|#
directive|define
name|SIGQUIT
value|3
end_define
begin_define
DECL|macro|SIGKILL
define|#
directive|define
name|SIGKILL
value|9
end_define
begin_define
DECL|macro|SIGPIPE
define|#
directive|define
name|SIGPIPE
value|13
end_define
begin_define
DECL|macro|SIGALRM
define|#
directive|define
name|SIGALRM
value|14
end_define
begin_define
DECL|macro|SIGCHLD
define|#
directive|define
name|SIGCHLD
value|17
end_define
begin_define
DECL|macro|F_GETFD
define|#
directive|define
name|F_GETFD
value|1
end_define
begin_define
DECL|macro|F_SETFD
define|#
directive|define
name|F_SETFD
value|2
end_define
begin_define
DECL|macro|FD_CLOEXEC
define|#
directive|define
name|FD_CLOEXEC
value|0x1
end_define
begin_define
DECL|macro|EAFNOSUPPORT
define|#
directive|define
name|EAFNOSUPPORT
value|WSAEAFNOSUPPORT
end_define
begin_define
DECL|macro|ECONNABORTED
define|#
directive|define
name|ECONNABORTED
value|WSAECONNABORTED
end_define
begin_struct
DECL|struct|passwd
struct|struct
name|passwd
block|{
DECL|member|pw_name
name|char
modifier|*
name|pw_name
decl_stmt|;
DECL|member|pw_gecos
name|char
modifier|*
name|pw_gecos
decl_stmt|;
DECL|member|pw_dir
name|char
modifier|*
name|pw_dir
decl_stmt|;
block|}
struct|;
end_struct
begin_function_decl
specifier|extern
name|char
modifier|*
name|getpass
parameter_list|(
specifier|const
name|char
modifier|*
name|prompt
parameter_list|)
function_decl|;
end_function_decl
begin_typedef
DECL|typedef|sig_handler_t
typedef|typedef
name|void
function_decl|(
name|__cdecl
modifier|*
name|sig_handler_t
function_decl|)
parameter_list|(
name|int
parameter_list|)
function_decl|;
end_typedef
begin_struct
DECL|struct|sigaction
struct|struct
name|sigaction
block|{
DECL|member|sa_handler
name|sig_handler_t
name|sa_handler
decl_stmt|;
DECL|member|sa_flags
name|unsigned
name|sa_flags
decl_stmt|;
block|}
struct|;
end_struct
begin_define
DECL|macro|sigemptyset
define|#
directive|define
name|sigemptyset
parameter_list|(
name|x
parameter_list|)
value|(void)0
end_define
begin_define
DECL|macro|SA_RESTART
define|#
directive|define
name|SA_RESTART
value|0
end_define
begin_struct
DECL|struct|itimerval
struct|struct
name|itimerval
block|{
DECL|member|it_value
DECL|member|it_interval
name|struct
name|timeval
name|it_value
decl_stmt|,
name|it_interval
decl_stmt|;
block|}
struct|;
end_struct
begin_define
DECL|macro|ITIMER_REAL
define|#
directive|define
name|ITIMER_REAL
value|0
end_define
begin_comment
comment|/*  * sanitize preprocessor namespace polluted by Windows headers defining  * macros which collide with git local versions  */
end_comment
begin_undef
DECL|macro|HELP_COMMAND
undef|#
directive|undef
name|HELP_COMMAND
end_undef
begin_comment
DECL|macro|HELP_COMMAND
comment|/* from winuser.h */
end_comment
begin_comment
comment|/*  * trivial stubs  */
end_comment
begin_function
DECL|function|readlink
specifier|static
specifier|inline
name|int
name|readlink
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|,
name|char
modifier|*
name|buf
parameter_list|,
name|size_t
name|bufsiz
parameter_list|)
block|{
name|errno
operator|=
name|ENOSYS
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
end_function
begin_function
DECL|function|symlink
specifier|static
specifier|inline
name|int
name|symlink
parameter_list|(
specifier|const
name|char
modifier|*
name|oldpath
parameter_list|,
specifier|const
name|char
modifier|*
name|newpath
parameter_list|)
block|{
name|errno
operator|=
name|ENOSYS
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
end_function
begin_function
DECL|function|fchmod
specifier|static
specifier|inline
name|int
name|fchmod
parameter_list|(
name|int
name|fildes
parameter_list|,
name|mode_t
name|mode
parameter_list|)
block|{
name|errno
operator|=
name|ENOSYS
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
end_function
begin_function
DECL|function|fork
specifier|static
specifier|inline
name|pid_t
name|fork
parameter_list|(
name|void
parameter_list|)
block|{
name|errno
operator|=
name|ENOSYS
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
end_function
begin_function
DECL|function|alarm
specifier|static
specifier|inline
name|unsigned
name|int
name|alarm
parameter_list|(
name|unsigned
name|int
name|seconds
parameter_list|)
block|{
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|fsync
specifier|static
specifier|inline
name|int
name|fsync
parameter_list|(
name|int
name|fd
parameter_list|)
block|{
return|return
name|_commit
argument_list|(
name|fd
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|getppid
specifier|static
specifier|inline
name|pid_t
name|getppid
parameter_list|(
name|void
parameter_list|)
block|{
return|return
literal|1
return|;
block|}
end_function
begin_function
DECL|function|sync
specifier|static
specifier|inline
name|void
name|sync
parameter_list|(
name|void
parameter_list|)
block|{}
end_function
begin_function
DECL|function|getuid
specifier|static
specifier|inline
name|uid_t
name|getuid
parameter_list|(
name|void
parameter_list|)
block|{
return|return
literal|1
return|;
block|}
end_function
begin_function
DECL|function|getpwnam
specifier|static
specifier|inline
name|struct
name|passwd
modifier|*
name|getpwnam
parameter_list|(
specifier|const
name|char
modifier|*
name|name
parameter_list|)
block|{
return|return
name|NULL
return|;
block|}
end_function
begin_function
DECL|function|fcntl
specifier|static
specifier|inline
name|int
name|fcntl
parameter_list|(
name|int
name|fd
parameter_list|,
name|int
name|cmd
parameter_list|,
modifier|...
parameter_list|)
block|{
if|if
condition|(
name|cmd
operator|==
name|F_GETFD
operator|||
name|cmd
operator|==
name|F_SETFD
condition|)
return|return
literal|0
return|;
name|errno
operator|=
name|EINVAL
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
end_function
begin_comment
comment|/* bash cannot reliably detect negative return codes as failure */
end_comment
begin_define
DECL|macro|exit
define|#
directive|define
name|exit
parameter_list|(
name|code
parameter_list|)
value|exit((code)& 0xff)
end_define
begin_comment
comment|/*  * simple adaptors  */
end_comment
begin_function
DECL|function|mingw_mkdir
specifier|static
specifier|inline
name|int
name|mingw_mkdir
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|,
name|int
name|mode
parameter_list|)
block|{
return|return
name|mkdir
argument_list|(
name|path
argument_list|)
return|;
block|}
end_function
begin_define
DECL|macro|mkdir
define|#
directive|define
name|mkdir
value|mingw_mkdir
end_define
begin_define
DECL|macro|WNOHANG
define|#
directive|define
name|WNOHANG
value|1
end_define
begin_function_decl
name|pid_t
name|waitpid
parameter_list|(
name|pid_t
name|pid
parameter_list|,
name|int
modifier|*
name|status
parameter_list|,
name|unsigned
name|options
parameter_list|)
function_decl|;
end_function_decl
begin_define
DECL|macro|kill
define|#
directive|define
name|kill
value|mingw_kill
end_define
begin_function_decl
name|int
name|mingw_kill
parameter_list|(
name|pid_t
name|pid
parameter_list|,
name|int
name|sig
parameter_list|)
function_decl|;
end_function_decl
begin_ifndef
ifndef|#
directive|ifndef
name|NO_OPENSSL
end_ifndef
begin_include
include|#
directive|include
file|<openssl/ssl.h>
end_include
begin_function
DECL|function|mingw_SSL_set_fd
specifier|static
specifier|inline
name|int
name|mingw_SSL_set_fd
parameter_list|(
name|SSL
modifier|*
name|ssl
parameter_list|,
name|int
name|fd
parameter_list|)
block|{
return|return
name|SSL_set_fd
argument_list|(
name|ssl
argument_list|,
name|_get_osfhandle
argument_list|(
name|fd
argument_list|)
argument_list|)
return|;
block|}
end_function
begin_define
DECL|macro|SSL_set_fd
define|#
directive|define
name|SSL_set_fd
value|mingw_SSL_set_fd
end_define
begin_function
DECL|function|mingw_SSL_set_rfd
specifier|static
specifier|inline
name|int
name|mingw_SSL_set_rfd
parameter_list|(
name|SSL
modifier|*
name|ssl
parameter_list|,
name|int
name|fd
parameter_list|)
block|{
return|return
name|SSL_set_rfd
argument_list|(
name|ssl
argument_list|,
name|_get_osfhandle
argument_list|(
name|fd
argument_list|)
argument_list|)
return|;
block|}
end_function
begin_define
DECL|macro|SSL_set_rfd
define|#
directive|define
name|SSL_set_rfd
value|mingw_SSL_set_rfd
end_define
begin_function
DECL|function|mingw_SSL_set_wfd
specifier|static
specifier|inline
name|int
name|mingw_SSL_set_wfd
parameter_list|(
name|SSL
modifier|*
name|ssl
parameter_list|,
name|int
name|fd
parameter_list|)
block|{
return|return
name|SSL_set_wfd
argument_list|(
name|ssl
argument_list|,
name|_get_osfhandle
argument_list|(
name|fd
argument_list|)
argument_list|)
return|;
block|}
end_function
begin_define
DECL|macro|SSL_set_wfd
define|#
directive|define
name|SSL_set_wfd
value|mingw_SSL_set_wfd
end_define
begin_endif
endif|#
directive|endif
end_endif
begin_comment
comment|/*  * implementations of missing functions  */
end_comment
begin_function_decl
name|int
name|pipe
parameter_list|(
name|int
name|filedes
index|[
literal|2
index|]
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|unsigned
name|int
name|sleep
parameter_list|(
name|unsigned
name|int
name|seconds
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|int
name|mkstemp
parameter_list|(
name|char
modifier|*
name|template
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|int
name|gettimeofday
parameter_list|(
name|struct
name|timeval
modifier|*
name|tv
parameter_list|,
name|void
modifier|*
name|tz
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|struct
name|tm
modifier|*
name|gmtime_r
parameter_list|(
specifier|const
name|time_t
modifier|*
name|timep
parameter_list|,
name|struct
name|tm
modifier|*
name|result
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|struct
name|tm
modifier|*
name|localtime_r
parameter_list|(
specifier|const
name|time_t
modifier|*
name|timep
parameter_list|,
name|struct
name|tm
modifier|*
name|result
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|int
name|getpagesize
parameter_list|(
name|void
parameter_list|)
function_decl|;
end_function_decl
begin_comment
comment|/* defined in MinGW's libgcc.a */
end_comment
begin_function_decl
name|struct
name|passwd
modifier|*
name|getpwuid
parameter_list|(
name|uid_t
name|uid
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|int
name|setitimer
parameter_list|(
name|int
name|type
parameter_list|,
name|struct
name|itimerval
modifier|*
name|in
parameter_list|,
name|struct
name|itimerval
modifier|*
name|out
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|int
name|sigaction
parameter_list|(
name|int
name|sig
parameter_list|,
name|struct
name|sigaction
modifier|*
name|in
parameter_list|,
name|struct
name|sigaction
modifier|*
name|out
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|int
name|link
parameter_list|(
specifier|const
name|char
modifier|*
name|oldpath
parameter_list|,
specifier|const
name|char
modifier|*
name|newpath
parameter_list|)
function_decl|;
end_function_decl
begin_comment
comment|/*  * replacements of existing functions  */
end_comment
begin_function_decl
name|int
name|mingw_unlink
parameter_list|(
specifier|const
name|char
modifier|*
name|pathname
parameter_list|)
function_decl|;
end_function_decl
begin_define
DECL|macro|unlink
define|#
directive|define
name|unlink
value|mingw_unlink
end_define
begin_function_decl
name|int
name|mingw_rmdir
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|)
function_decl|;
end_function_decl
begin_define
DECL|macro|rmdir
define|#
directive|define
name|rmdir
value|mingw_rmdir
end_define
begin_function_decl
name|int
name|mingw_open
parameter_list|(
specifier|const
name|char
modifier|*
name|filename
parameter_list|,
name|int
name|oflags
parameter_list|,
modifier|...
parameter_list|)
function_decl|;
end_function_decl
begin_define
DECL|macro|open
define|#
directive|define
name|open
value|mingw_open
end_define
begin_function_decl
name|ssize_t
name|mingw_write
parameter_list|(
name|int
name|fd
parameter_list|,
specifier|const
name|void
modifier|*
name|buf
parameter_list|,
name|size_t
name|count
parameter_list|)
function_decl|;
end_function_decl
begin_define
DECL|macro|write
define|#
directive|define
name|write
value|mingw_write
end_define
begin_function_decl
name|FILE
modifier|*
name|mingw_fopen
parameter_list|(
specifier|const
name|char
modifier|*
name|filename
parameter_list|,
specifier|const
name|char
modifier|*
name|otype
parameter_list|)
function_decl|;
end_function_decl
begin_define
DECL|macro|fopen
define|#
directive|define
name|fopen
value|mingw_fopen
end_define
begin_function_decl
name|FILE
modifier|*
name|mingw_freopen
parameter_list|(
specifier|const
name|char
modifier|*
name|filename
parameter_list|,
specifier|const
name|char
modifier|*
name|otype
parameter_list|,
name|FILE
modifier|*
name|stream
parameter_list|)
function_decl|;
end_function_decl
begin_define
DECL|macro|freopen
define|#
directive|define
name|freopen
value|mingw_freopen
end_define
begin_function_decl
name|char
modifier|*
name|mingw_getcwd
parameter_list|(
name|char
modifier|*
name|pointer
parameter_list|,
name|int
name|len
parameter_list|)
function_decl|;
end_function_decl
begin_define
DECL|macro|getcwd
define|#
directive|define
name|getcwd
value|mingw_getcwd
end_define
begin_function_decl
name|char
modifier|*
name|mingw_getenv
parameter_list|(
specifier|const
name|char
modifier|*
name|name
parameter_list|)
function_decl|;
end_function_decl
begin_define
DECL|macro|getenv
define|#
directive|define
name|getenv
value|mingw_getenv
end_define
begin_function_decl
name|struct
name|hostent
modifier|*
name|mingw_gethostbyname
parameter_list|(
specifier|const
name|char
modifier|*
name|host
parameter_list|)
function_decl|;
end_function_decl
begin_define
DECL|macro|gethostbyname
define|#
directive|define
name|gethostbyname
value|mingw_gethostbyname
end_define
begin_function_decl
name|void
name|mingw_freeaddrinfo
parameter_list|(
name|struct
name|addrinfo
modifier|*
name|res
parameter_list|)
function_decl|;
end_function_decl
begin_define
DECL|macro|freeaddrinfo
define|#
directive|define
name|freeaddrinfo
value|mingw_freeaddrinfo
end_define
begin_function_decl
name|int
name|mingw_getaddrinfo
parameter_list|(
specifier|const
name|char
modifier|*
name|node
parameter_list|,
specifier|const
name|char
modifier|*
name|service
parameter_list|,
specifier|const
name|struct
name|addrinfo
modifier|*
name|hints
parameter_list|,
name|struct
name|addrinfo
modifier|*
modifier|*
name|res
parameter_list|)
function_decl|;
end_function_decl
begin_define
DECL|macro|getaddrinfo
define|#
directive|define
name|getaddrinfo
value|mingw_getaddrinfo
end_define
begin_function_decl
name|int
name|mingw_getnameinfo
parameter_list|(
specifier|const
name|struct
name|sockaddr
modifier|*
name|sa
parameter_list|,
name|socklen_t
name|salen
parameter_list|,
name|char
modifier|*
name|host
parameter_list|,
name|DWORD
name|hostlen
parameter_list|,
name|char
modifier|*
name|serv
parameter_list|,
name|DWORD
name|servlen
parameter_list|,
name|int
name|flags
parameter_list|)
function_decl|;
end_function_decl
begin_define
DECL|macro|getnameinfo
define|#
directive|define
name|getnameinfo
value|mingw_getnameinfo
end_define
begin_function_decl
name|int
name|mingw_socket
parameter_list|(
name|int
name|domain
parameter_list|,
name|int
name|type
parameter_list|,
name|int
name|protocol
parameter_list|)
function_decl|;
end_function_decl
begin_define
DECL|macro|socket
define|#
directive|define
name|socket
value|mingw_socket
end_define
begin_function_decl
name|int
name|mingw_connect
parameter_list|(
name|int
name|sockfd
parameter_list|,
name|struct
name|sockaddr
modifier|*
name|sa
parameter_list|,
name|size_t
name|sz
parameter_list|)
function_decl|;
end_function_decl
begin_define
DECL|macro|connect
define|#
directive|define
name|connect
value|mingw_connect
end_define
begin_function_decl
name|int
name|mingw_bind
parameter_list|(
name|int
name|sockfd
parameter_list|,
name|struct
name|sockaddr
modifier|*
name|sa
parameter_list|,
name|size_t
name|sz
parameter_list|)
function_decl|;
end_function_decl
begin_define
DECL|macro|bind
define|#
directive|define
name|bind
value|mingw_bind
end_define
begin_function_decl
name|int
name|mingw_setsockopt
parameter_list|(
name|int
name|sockfd
parameter_list|,
name|int
name|lvl
parameter_list|,
name|int
name|optname
parameter_list|,
name|void
modifier|*
name|optval
parameter_list|,
name|int
name|optlen
parameter_list|)
function_decl|;
end_function_decl
begin_define
DECL|macro|setsockopt
define|#
directive|define
name|setsockopt
value|mingw_setsockopt
end_define
begin_function_decl
name|int
name|mingw_listen
parameter_list|(
name|int
name|sockfd
parameter_list|,
name|int
name|backlog
parameter_list|)
function_decl|;
end_function_decl
begin_define
DECL|macro|listen
define|#
directive|define
name|listen
value|mingw_listen
end_define
begin_function_decl
name|int
name|mingw_accept
parameter_list|(
name|int
name|sockfd
parameter_list|,
name|struct
name|sockaddr
modifier|*
name|sa
parameter_list|,
name|socklen_t
modifier|*
name|sz
parameter_list|)
function_decl|;
end_function_decl
begin_define
DECL|macro|accept
define|#
directive|define
name|accept
value|mingw_accept
end_define
begin_function_decl
name|int
name|mingw_rename
parameter_list|(
specifier|const
name|char
modifier|*
parameter_list|,
specifier|const
name|char
modifier|*
parameter_list|)
function_decl|;
end_function_decl
begin_define
DECL|macro|rename
define|#
directive|define
name|rename
value|mingw_rename
end_define
begin_if
if|#
directive|if
name|defined
argument_list|(
name|USE_WIN32_MMAP
argument_list|)
operator|||
name|defined
argument_list|(
name|_MSC_VER
argument_list|)
end_if
begin_function_decl
name|int
name|mingw_getpagesize
parameter_list|(
name|void
parameter_list|)
function_decl|;
end_function_decl
begin_define
DECL|macro|getpagesize
define|#
directive|define
name|getpagesize
value|mingw_getpagesize
end_define
begin_endif
endif|#
directive|endif
end_endif
begin_struct
DECL|struct|rlimit
struct|struct
name|rlimit
block|{
DECL|member|rlim_cur
name|unsigned
name|int
name|rlim_cur
decl_stmt|;
block|}
struct|;
end_struct
begin_define
DECL|macro|RLIMIT_NOFILE
define|#
directive|define
name|RLIMIT_NOFILE
value|0
end_define
begin_function
DECL|function|getrlimit
specifier|static
specifier|inline
name|int
name|getrlimit
parameter_list|(
name|int
name|resource
parameter_list|,
name|struct
name|rlimit
modifier|*
name|rlp
parameter_list|)
block|{
if|if
condition|(
name|resource
operator|!=
name|RLIMIT_NOFILE
condition|)
block|{
name|errno
operator|=
name|EINVAL
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
name|rlp
operator|->
name|rlim_cur
operator|=
literal|2048
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_comment
comment|/* Use mingw_lstat() instead of lstat()/stat() and  * mingw_fstat() instead of fstat() on Windows.  */
end_comment
begin_define
DECL|macro|off_t
define|#
directive|define
name|off_t
value|off64_t
end_define
begin_define
DECL|macro|lseek
define|#
directive|define
name|lseek
value|_lseeki64
end_define
begin_ifndef
ifndef|#
directive|ifndef
name|ALREADY_DECLARED_STAT_FUNCS
end_ifndef
begin_define
DECL|macro|stat
define|#
directive|define
name|stat
value|_stati64
end_define
begin_function_decl
name|int
name|mingw_lstat
parameter_list|(
specifier|const
name|char
modifier|*
name|file_name
parameter_list|,
name|struct
name|stat
modifier|*
name|buf
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|int
name|mingw_stat
parameter_list|(
specifier|const
name|char
modifier|*
name|file_name
parameter_list|,
name|struct
name|stat
modifier|*
name|buf
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|int
name|mingw_fstat
parameter_list|(
name|int
name|fd
parameter_list|,
name|struct
name|stat
modifier|*
name|buf
parameter_list|)
function_decl|;
end_function_decl
begin_define
DECL|macro|fstat
define|#
directive|define
name|fstat
value|mingw_fstat
end_define
begin_define
DECL|macro|lstat
define|#
directive|define
name|lstat
value|mingw_lstat
end_define
begin_define
DECL|macro|_stati64
define|#
directive|define
name|_stati64
parameter_list|(
name|x
parameter_list|,
name|y
parameter_list|)
value|mingw_stat(x,y)
end_define
begin_endif
endif|#
directive|endif
end_endif
begin_function_decl
name|int
name|mingw_utime
parameter_list|(
specifier|const
name|char
modifier|*
name|file_name
parameter_list|,
specifier|const
name|struct
name|utimbuf
modifier|*
name|times
parameter_list|)
function_decl|;
end_function_decl
begin_define
DECL|macro|utime
define|#
directive|define
name|utime
value|mingw_utime
end_define
begin_function_decl
name|pid_t
name|mingw_spawnvpe
parameter_list|(
specifier|const
name|char
modifier|*
name|cmd
parameter_list|,
specifier|const
name|char
modifier|*
modifier|*
name|argv
parameter_list|,
name|char
modifier|*
modifier|*
name|env
parameter_list|,
specifier|const
name|char
modifier|*
name|dir
parameter_list|,
name|int
name|fhin
parameter_list|,
name|int
name|fhout
parameter_list|,
name|int
name|fherr
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|void
name|mingw_execvp
parameter_list|(
specifier|const
name|char
modifier|*
name|cmd
parameter_list|,
name|char
modifier|*
specifier|const
modifier|*
name|argv
parameter_list|)
function_decl|;
end_function_decl
begin_define
DECL|macro|execvp
define|#
directive|define
name|execvp
value|mingw_execvp
end_define
begin_function_decl
name|void
name|mingw_execv
parameter_list|(
specifier|const
name|char
modifier|*
name|cmd
parameter_list|,
name|char
modifier|*
specifier|const
modifier|*
name|argv
parameter_list|)
function_decl|;
end_function_decl
begin_define
DECL|macro|execv
define|#
directive|define
name|execv
value|mingw_execv
end_define
begin_function
DECL|function|git_ntohl
specifier|static
specifier|inline
name|unsigned
name|int
name|git_ntohl
parameter_list|(
name|unsigned
name|int
name|x
parameter_list|)
block|{
return|return
operator|(
name|unsigned
name|int
operator|)
name|ntohl
argument_list|(
name|x
argument_list|)
return|;
block|}
end_function
begin_define
DECL|macro|ntohl
define|#
directive|define
name|ntohl
value|git_ntohl
end_define
begin_function_decl
name|sig_handler_t
name|mingw_signal
parameter_list|(
name|int
name|sig
parameter_list|,
name|sig_handler_t
name|handler
parameter_list|)
function_decl|;
end_function_decl
begin_define
DECL|macro|signal
define|#
directive|define
name|signal
value|mingw_signal
end_define
begin_comment
comment|/*  * ANSI emulation wrappers  */
end_comment
begin_function_decl
name|int
name|winansi_fputs
parameter_list|(
specifier|const
name|char
modifier|*
name|str
parameter_list|,
name|FILE
modifier|*
name|stream
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|int
name|winansi_printf
parameter_list|(
specifier|const
name|char
modifier|*
name|format
parameter_list|,
modifier|...
parameter_list|)
function_decl|__attribute__
parameter_list|(
function_decl|(format
parameter_list|(
name|printf
parameter_list|,
function_decl|1
operator|,
function_decl|2
end_function_decl
begin_empty_stmt
unit|)))
empty_stmt|;
end_empty_stmt
begin_function_decl
name|int
name|winansi_fprintf
parameter_list|(
name|FILE
modifier|*
name|stream
parameter_list|,
specifier|const
name|char
modifier|*
name|format
parameter_list|,
modifier|...
parameter_list|)
function_decl|__attribute__
parameter_list|(
function_decl|(format
parameter_list|(
name|printf
parameter_list|,
function_decl|2
operator|,
function_decl|3
end_function_decl
begin_empty_stmt
unit|)))
empty_stmt|;
end_empty_stmt
begin_define
DECL|macro|fputs
define|#
directive|define
name|fputs
value|winansi_fputs
end_define
begin_define
DECL|macro|printf
define|#
directive|define
name|printf
parameter_list|(
modifier|...
parameter_list|)
value|winansi_printf(__VA_ARGS__)
end_define
begin_define
DECL|macro|fprintf
define|#
directive|define
name|fprintf
parameter_list|(
modifier|...
parameter_list|)
value|winansi_fprintf(__VA_ARGS__)
end_define
begin_comment
comment|/*  * git specific compatibility  */
end_comment
begin_define
DECL|macro|has_dos_drive_prefix
define|#
directive|define
name|has_dos_drive_prefix
parameter_list|(
name|path
parameter_list|)
value|(isalpha(*(path))&& (path)[1] == ':')
end_define
begin_define
DECL|macro|is_dir_sep
define|#
directive|define
name|is_dir_sep
parameter_list|(
name|c
parameter_list|)
value|((c) == '/' || (c) == '\\')
end_define
begin_define
DECL|macro|PATH_SEP
define|#
directive|define
name|PATH_SEP
value|';'
end_define
begin_define
DECL|macro|PRIuMAX
define|#
directive|define
name|PRIuMAX
value|"I64u"
end_define
begin_function_decl
name|void
name|mingw_open_html
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|)
function_decl|;
end_function_decl
begin_define
DECL|macro|open_html
define|#
directive|define
name|open_html
value|mingw_open_html
end_define
begin_comment
comment|/*  * helpers  */
end_comment
begin_function_decl
name|char
modifier|*
modifier|*
name|make_augmented_environ
parameter_list|(
specifier|const
name|char
modifier|*
specifier|const
modifier|*
name|vars
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|void
name|free_environ
parameter_list|(
name|char
modifier|*
modifier|*
name|env
parameter_list|)
function_decl|;
end_function_decl
begin_comment
comment|/*  * A replacement of main() that ensures that argv[0] has a path  * and that default fmode and std(in|out|err) are in binary mode  */
end_comment
begin_define
DECL|macro|main
define|#
directive|define
name|main
parameter_list|(
name|c
parameter_list|,
name|v
parameter_list|)
value|dummy_decl_mingw_main(); \ static int mingw_main(); \ int main(int argc, const char **argv) \ { \ 	extern CRITICAL_SECTION pinfo_cs; \ 	_fmode = _O_BINARY; \ 	_setmode(_fileno(stdin), _O_BINARY); \ 	_setmode(_fileno(stdout), _O_BINARY); \ 	_setmode(_fileno(stderr), _O_BINARY); \ 	argv[0] = xstrdup(_pgmptr); \ 	InitializeCriticalSection(&pinfo_cs); \ 	return mingw_main(argc, argv); \ } \ static int mingw_main(c,v)
end_define
begin_comment
comment|/*  * Used by Pthread API implementation for Windows  */
end_comment
begin_function_decl
specifier|extern
name|int
name|err_win_to_posix
parameter_list|(
name|DWORD
name|winerr
parameter_list|)
function_decl|;
end_function_decl
end_unit
