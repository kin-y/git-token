begin_unit
begin_comment
comment|/* Emulation for poll(2)    Contributed by Paolo Bonzini.     Copyright 2001-2003, 2006-2011 Free Software Foundation, Inc.     This file is part of gnulib.     This program is free software; you can redistribute it and/or modify    it under the terms of the GNU General Public License as published by    the Free Software Foundation; either version 2, or (at your option)    any later version.     This program is distributed in the hope that it will be useful,    but WITHOUT ANY WARRANTY; without even the implied warranty of    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the    GNU General Public License for more details.     You should have received a copy of the GNU General Public License along    with this program; if not, write to the Free Software Foundation,    Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  */
end_comment
begin_comment
comment|/* Tell gcc not to warn about the (nfd< 0) tests, below.  */
end_comment
begin_if
if|#
directive|if
operator|(
name|__GNUC__
operator|==
literal|4
operator|&&
literal|3
operator|<=
name|__GNUC_MINOR__
operator|)
operator|||
literal|4
operator|<
name|__GNUC__
end_if
begin_pragma
pragma|#
directive|pragma
name|GCC
name|diagnostic
name|ignored
literal|"-Wtype-limits"
end_pragma
begin_endif
endif|#
directive|endif
end_endif
begin_if
if|#
directive|if
name|defined
argument_list|(
name|WIN32
argument_list|)
end_if
begin_include
include|#
directive|include
file|<malloc.h>
end_include
begin_endif
endif|#
directive|endif
end_endif
begin_include
include|#
directive|include
file|<sys/types.h>
end_include
begin_comment
comment|/* Specification.  */
end_comment
begin_include
include|#
directive|include
file|<poll.h>
end_include
begin_include
include|#
directive|include
file|<errno.h>
end_include
begin_include
include|#
directive|include
file|<limits.h>
end_include
begin_include
include|#
directive|include
file|<assert.h>
end_include
begin_if
if|#
directive|if
operator|(
name|defined
name|_WIN32
operator|||
name|defined
name|__WIN32__
operator|)
operator|&&
operator|!
name|defined
name|__CYGWIN__
end_if
begin_define
DECL|macro|WIN32_NATIVE
define|#
directive|define
name|WIN32_NATIVE
end_define
begin_if
if|#
directive|if
name|defined
argument_list|(
name|_MSC_VER
argument_list|)
operator|&&
operator|!
name|defined
argument_list|(
name|_WIN32_WINNT
argument_list|)
end_if
begin_define
DECL|macro|_WIN32_WINNT
define|#
directive|define
name|_WIN32_WINNT
value|0x0502
end_define
begin_endif
endif|#
directive|endif
end_endif
begin_include
include|#
directive|include
file|<winsock2.h>
end_include
begin_include
include|#
directive|include
file|<windows.h>
end_include
begin_include
include|#
directive|include
file|<io.h>
end_include
begin_include
include|#
directive|include
file|<stdio.h>
end_include
begin_include
include|#
directive|include
file|<conio.h>
end_include
begin_else
else|#
directive|else
end_else
begin_include
include|#
directive|include
file|<sys/time.h>
end_include
begin_include
include|#
directive|include
file|<sys/socket.h>
end_include
begin_ifndef
ifndef|#
directive|ifndef
name|NO_SYS_SELECT_H
end_ifndef
begin_include
include|#
directive|include
file|<sys/select.h>
end_include
begin_endif
endif|#
directive|endif
end_endif
begin_include
include|#
directive|include
file|<unistd.h>
end_include
begin_endif
endif|#
directive|endif
end_endif
begin_ifdef
ifdef|#
directive|ifdef
name|HAVE_SYS_IOCTL_H
end_ifdef
begin_include
include|#
directive|include
file|<sys/ioctl.h>
end_include
begin_endif
endif|#
directive|endif
end_endif
begin_ifdef
ifdef|#
directive|ifdef
name|HAVE_SYS_FILIO_H
end_ifdef
begin_include
include|#
directive|include
file|<sys/filio.h>
end_include
begin_endif
endif|#
directive|endif
end_endif
begin_include
include|#
directive|include
file|<time.h>
end_include
begin_ifndef
ifndef|#
directive|ifndef
name|INFTIM
end_ifndef
begin_define
DECL|macro|INFTIM
define|#
directive|define
name|INFTIM
value|(-1)
end_define
begin_endif
endif|#
directive|endif
end_endif
begin_comment
comment|/* BeOS does not have MSG_PEEK.  */
end_comment
begin_ifndef
ifndef|#
directive|ifndef
name|MSG_PEEK
end_ifndef
begin_define
DECL|macro|MSG_PEEK
define|#
directive|define
name|MSG_PEEK
value|0
end_define
begin_endif
endif|#
directive|endif
end_endif
begin_ifdef
ifdef|#
directive|ifdef
name|WIN32_NATIVE
end_ifdef
begin_define
DECL|macro|IsConsoleHandle
define|#
directive|define
name|IsConsoleHandle
parameter_list|(
name|h
parameter_list|)
value|(((long) (h)& 3) == 3)
end_define
begin_function
specifier|static
name|BOOL
DECL|function|IsSocketHandle
name|IsSocketHandle
parameter_list|(
name|HANDLE
name|h
parameter_list|)
block|{
name|WSANETWORKEVENTS
name|ev
decl_stmt|;
if|if
condition|(
name|IsConsoleHandle
argument_list|(
name|h
argument_list|)
condition|)
return|return
name|FALSE
return|;
comment|/* Under Wine, it seems that getsockopt returns 0 for pipes too.      WSAEnumNetworkEvents instead distinguishes the two correctly.  */
name|ev
operator|.
name|lNetworkEvents
operator|=
literal|0xDEADBEEF
expr_stmt|;
name|WSAEnumNetworkEvents
argument_list|(
operator|(
name|SOCKET
operator|)
name|h
argument_list|,
name|NULL
argument_list|,
operator|&
name|ev
argument_list|)
expr_stmt|;
return|return
name|ev
operator|.
name|lNetworkEvents
operator|!=
literal|0xDEADBEEF
return|;
block|}
end_function
begin_comment
comment|/* Declare data structures for ntdll functions.  */
end_comment
begin_typedef
DECL|struct|_FILE_PIPE_LOCAL_INFORMATION
typedef|typedef
struct|struct
name|_FILE_PIPE_LOCAL_INFORMATION
block|{
DECL|member|NamedPipeType
name|ULONG
name|NamedPipeType
decl_stmt|;
DECL|member|NamedPipeConfiguration
name|ULONG
name|NamedPipeConfiguration
decl_stmt|;
DECL|member|MaximumInstances
name|ULONG
name|MaximumInstances
decl_stmt|;
DECL|member|CurrentInstances
name|ULONG
name|CurrentInstances
decl_stmt|;
DECL|member|InboundQuota
name|ULONG
name|InboundQuota
decl_stmt|;
DECL|member|ReadDataAvailable
name|ULONG
name|ReadDataAvailable
decl_stmt|;
DECL|member|OutboundQuota
name|ULONG
name|OutboundQuota
decl_stmt|;
DECL|member|WriteQuotaAvailable
name|ULONG
name|WriteQuotaAvailable
decl_stmt|;
DECL|member|NamedPipeState
name|ULONG
name|NamedPipeState
decl_stmt|;
DECL|member|NamedPipeEnd
name|ULONG
name|NamedPipeEnd
decl_stmt|;
block|}
DECL|typedef|FILE_PIPE_LOCAL_INFORMATION
DECL|typedef|PFILE_PIPE_LOCAL_INFORMATION
name|FILE_PIPE_LOCAL_INFORMATION
operator|,
typedef|*
name|PFILE_PIPE_LOCAL_INFORMATION
typedef|;
end_typedef
begin_typedef
DECL|struct|_IO_STATUS_BLOCK
typedef|typedef
struct|struct
name|_IO_STATUS_BLOCK
block|{
union|union
block|{
DECL|member|Status
name|DWORD
name|Status
decl_stmt|;
DECL|member|Pointer
name|PVOID
name|Pointer
decl_stmt|;
block|}
DECL|member|u
name|u
union|;
DECL|member|Information
name|ULONG_PTR
name|Information
decl_stmt|;
block|}
DECL|typedef|IO_STATUS_BLOCK
DECL|typedef|PIO_STATUS_BLOCK
name|IO_STATUS_BLOCK
operator|,
typedef|*
name|PIO_STATUS_BLOCK
typedef|;
end_typedef
begin_typedef
DECL|enum|_FILE_INFORMATION_CLASS
typedef|typedef
enum|enum
name|_FILE_INFORMATION_CLASS
block|{
DECL|enumerator|FilePipeLocalInformation
name|FilePipeLocalInformation
init|=
literal|24
block|}
DECL|typedef|FILE_INFORMATION_CLASS
DECL|typedef|PFILE_INFORMATION_CLASS
name|FILE_INFORMATION_CLASS
operator|,
typedef|*
name|PFILE_INFORMATION_CLASS
typedef|;
end_typedef
begin_typedef
DECL|typedef|PNtQueryInformationFile
typedef|typedef
name|DWORD
function_decl|(
name|WINAPI
modifier|*
name|PNtQueryInformationFile
function_decl|)
parameter_list|(
name|HANDLE
parameter_list|,
name|IO_STATUS_BLOCK
modifier|*
parameter_list|,
name|VOID
modifier|*
parameter_list|,
name|ULONG
parameter_list|,
name|FILE_INFORMATION_CLASS
parameter_list|)
function_decl|;
end_typedef
begin_ifndef
ifndef|#
directive|ifndef
name|PIPE_BUF
end_ifndef
begin_define
DECL|macro|PIPE_BUF
define|#
directive|define
name|PIPE_BUF
value|512
end_define
begin_endif
endif|#
directive|endif
end_endif
begin_comment
comment|/* Compute revents values for file handle H.  If some events cannot happen    for the handle, eliminate them from *P_SOUGHT.  */
end_comment
begin_function
specifier|static
name|int
DECL|function|win32_compute_revents
name|win32_compute_revents
parameter_list|(
name|HANDLE
name|h
parameter_list|,
name|int
modifier|*
name|p_sought
parameter_list|)
block|{
name|int
name|i
decl_stmt|,
name|ret
decl_stmt|,
name|happened
decl_stmt|;
name|INPUT_RECORD
modifier|*
name|irbuffer
decl_stmt|;
name|DWORD
name|avail
decl_stmt|,
name|nbuffer
decl_stmt|;
name|BOOL
name|bRet
decl_stmt|;
name|IO_STATUS_BLOCK
name|iosb
decl_stmt|;
name|FILE_PIPE_LOCAL_INFORMATION
name|fpli
decl_stmt|;
specifier|static
name|PNtQueryInformationFile
name|NtQueryInformationFile
decl_stmt|;
specifier|static
name|BOOL
name|once_only
decl_stmt|;
switch|switch
condition|(
name|GetFileType
argument_list|(
name|h
argument_list|)
condition|)
block|{
case|case
name|FILE_TYPE_PIPE
case|:
if|if
condition|(
operator|!
name|once_only
condition|)
block|{
name|NtQueryInformationFile
operator|=
operator|(
name|PNtQueryInformationFile
operator|)
name|GetProcAddress
argument_list|(
name|GetModuleHandle
argument_list|(
literal|"ntdll.dll"
argument_list|)
argument_list|,
literal|"NtQueryInformationFile"
argument_list|)
expr_stmt|;
name|once_only
operator|=
name|TRUE
expr_stmt|;
block|}
name|happened
operator|=
literal|0
expr_stmt|;
if|if
condition|(
name|PeekNamedPipe
argument_list|(
name|h
argument_list|,
name|NULL
argument_list|,
literal|0
argument_list|,
name|NULL
argument_list|,
operator|&
name|avail
argument_list|,
name|NULL
argument_list|)
operator|!=
literal|0
condition|)
block|{
if|if
condition|(
name|avail
condition|)
name|happened
operator||=
operator|*
name|p_sought
operator|&
operator|(
name|POLLIN
operator||
name|POLLRDNORM
operator|)
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|GetLastError
argument_list|()
operator|==
name|ERROR_BROKEN_PIPE
condition|)
name|happened
operator||=
name|POLLHUP
expr_stmt|;
else|else
block|{
comment|/* It was the write-end of the pipe.  Check if it is writable. 	     If NtQueryInformationFile fails, optimistically assume the pipe is 	     writable.  This could happen on Win9x, where NtQueryInformationFile 	     is not available, or if we inherit a pipe that doesn't permit 	     FILE_READ_ATTRIBUTES access on the write end (I think this should 	     not happen since WinXP SP2; WINE seems fine too).  Otherwise, 	     ensure that enough space is available for atomic writes.  */
name|memset
argument_list|(
operator|&
name|iosb
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
name|iosb
argument_list|)
argument_list|)
expr_stmt|;
name|memset
argument_list|(
operator|&
name|fpli
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
name|fpli
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|NtQueryInformationFile
operator|||
name|NtQueryInformationFile
argument_list|(
name|h
argument_list|,
operator|&
name|iosb
argument_list|,
operator|&
name|fpli
argument_list|,
sizeof|sizeof
argument_list|(
name|fpli
argument_list|)
argument_list|,
name|FilePipeLocalInformation
argument_list|)
operator|||
name|fpli
operator|.
name|WriteQuotaAvailable
operator|>=
name|PIPE_BUF
operator|||
operator|(
name|fpli
operator|.
name|OutboundQuota
operator|<
name|PIPE_BUF
operator|&&
name|fpli
operator|.
name|WriteQuotaAvailable
operator|==
name|fpli
operator|.
name|OutboundQuota
operator|)
condition|)
name|happened
operator||=
operator|*
name|p_sought
operator|&
operator|(
name|POLLOUT
operator||
name|POLLWRNORM
operator||
name|POLLWRBAND
operator|)
expr_stmt|;
block|}
return|return
name|happened
return|;
case|case
name|FILE_TYPE_CHAR
case|:
name|ret
operator|=
name|WaitForSingleObject
argument_list|(
name|h
argument_list|,
literal|0
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|IsConsoleHandle
argument_list|(
name|h
argument_list|)
condition|)
return|return
name|ret
operator|==
name|WAIT_OBJECT_0
condition|?
operator|*
name|p_sought
operator|&
operator|~
operator|(
name|POLLPRI
operator||
name|POLLRDBAND
operator|)
else|:
literal|0
return|;
name|nbuffer
operator|=
name|avail
operator|=
literal|0
expr_stmt|;
name|bRet
operator|=
name|GetNumberOfConsoleInputEvents
argument_list|(
name|h
argument_list|,
operator|&
name|nbuffer
argument_list|)
expr_stmt|;
if|if
condition|(
name|bRet
condition|)
block|{
comment|/* Input buffer.  */
operator|*
name|p_sought
operator|&=
name|POLLIN
operator||
name|POLLRDNORM
expr_stmt|;
if|if
condition|(
name|nbuffer
operator|==
literal|0
condition|)
return|return
name|POLLHUP
return|;
if|if
condition|(
operator|!
operator|*
name|p_sought
condition|)
return|return
literal|0
return|;
name|irbuffer
operator|=
operator|(
name|INPUT_RECORD
operator|*
operator|)
name|alloca
argument_list|(
name|nbuffer
operator|*
sizeof|sizeof
argument_list|(
name|INPUT_RECORD
argument_list|)
argument_list|)
expr_stmt|;
name|bRet
operator|=
name|PeekConsoleInput
argument_list|(
name|h
argument_list|,
name|irbuffer
argument_list|,
name|nbuffer
argument_list|,
operator|&
name|avail
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|bRet
operator|||
name|avail
operator|==
literal|0
condition|)
return|return
name|POLLHUP
return|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|avail
condition|;
name|i
operator|++
control|)
if|if
condition|(
name|irbuffer
index|[
name|i
index|]
operator|.
name|EventType
operator|==
name|KEY_EVENT
condition|)
return|return
operator|*
name|p_sought
return|;
return|return
literal|0
return|;
block|}
else|else
block|{
comment|/* Screen buffer.  */
operator|*
name|p_sought
operator|&=
name|POLLOUT
operator||
name|POLLWRNORM
operator||
name|POLLWRBAND
expr_stmt|;
return|return
operator|*
name|p_sought
return|;
block|}
default|default:
name|ret
operator|=
name|WaitForSingleObject
argument_list|(
name|h
argument_list|,
literal|0
argument_list|)
expr_stmt|;
if|if
condition|(
name|ret
operator|==
name|WAIT_OBJECT_0
condition|)
return|return
operator|*
name|p_sought
operator|&
operator|~
operator|(
name|POLLPRI
operator||
name|POLLRDBAND
operator|)
return|;
return|return
operator|*
name|p_sought
operator|&
operator|(
name|POLLOUT
operator||
name|POLLWRNORM
operator||
name|POLLWRBAND
operator|)
return|;
block|}
block|}
end_function
begin_comment
comment|/* Convert fd_sets returned by select into revents values.  */
end_comment
begin_function
specifier|static
name|int
DECL|function|win32_compute_revents_socket
name|win32_compute_revents_socket
parameter_list|(
name|SOCKET
name|h
parameter_list|,
name|int
name|sought
parameter_list|,
name|long
name|lNetworkEvents
parameter_list|)
block|{
name|int
name|happened
init|=
literal|0
decl_stmt|;
if|if
condition|(
operator|(
name|lNetworkEvents
operator|&
operator|(
name|FD_READ
operator||
name|FD_ACCEPT
operator||
name|FD_CLOSE
operator|)
operator|)
operator|==
name|FD_ACCEPT
condition|)
name|happened
operator||=
operator|(
name|POLLIN
operator||
name|POLLRDNORM
operator|)
operator|&
name|sought
expr_stmt|;
elseif|else
if|if
condition|(
name|lNetworkEvents
operator|&
operator|(
name|FD_READ
operator||
name|FD_ACCEPT
operator||
name|FD_CLOSE
operator|)
condition|)
block|{
name|int
name|r
decl_stmt|,
name|error
decl_stmt|;
name|char
name|data
index|[
literal|64
index|]
decl_stmt|;
name|WSASetLastError
argument_list|(
literal|0
argument_list|)
expr_stmt|;
name|r
operator|=
name|recv
argument_list|(
name|h
argument_list|,
name|data
argument_list|,
sizeof|sizeof
argument_list|(
name|data
argument_list|)
argument_list|,
name|MSG_PEEK
argument_list|)
expr_stmt|;
name|error
operator|=
name|WSAGetLastError
argument_list|()
expr_stmt|;
name|WSASetLastError
argument_list|(
literal|0
argument_list|)
expr_stmt|;
if|if
condition|(
name|r
operator|>
literal|0
operator|||
name|error
operator|==
name|WSAENOTCONN
condition|)
name|happened
operator||=
operator|(
name|POLLIN
operator||
name|POLLRDNORM
operator|)
operator|&
name|sought
expr_stmt|;
comment|/* Distinguish hung-up sockets from other errors.  */
elseif|else
if|if
condition|(
name|r
operator|==
literal|0
operator|||
name|error
operator|==
name|WSAESHUTDOWN
operator|||
name|error
operator|==
name|WSAECONNRESET
operator|||
name|error
operator|==
name|WSAECONNABORTED
operator|||
name|error
operator|==
name|WSAENETRESET
condition|)
name|happened
operator||=
name|POLLHUP
expr_stmt|;
else|else
name|happened
operator||=
name|POLLERR
expr_stmt|;
block|}
if|if
condition|(
name|lNetworkEvents
operator|&
operator|(
name|FD_WRITE
operator||
name|FD_CONNECT
operator|)
condition|)
name|happened
operator||=
operator|(
name|POLLOUT
operator||
name|POLLWRNORM
operator||
name|POLLWRBAND
operator|)
operator|&
name|sought
expr_stmt|;
if|if
condition|(
name|lNetworkEvents
operator|&
name|FD_OOB
condition|)
name|happened
operator||=
operator|(
name|POLLPRI
operator||
name|POLLRDBAND
operator|)
operator|&
name|sought
expr_stmt|;
return|return
name|happened
return|;
block|}
end_function
begin_else
else|#
directive|else
end_else
begin_comment
comment|/* !MinGW */
end_comment
begin_comment
comment|/* Convert select(2) returned fd_sets into poll(2) revents values.  */
end_comment
begin_function
specifier|static
name|int
DECL|function|compute_revents
name|compute_revents
parameter_list|(
name|int
name|fd
parameter_list|,
name|int
name|sought
parameter_list|,
name|fd_set
modifier|*
name|rfds
parameter_list|,
name|fd_set
modifier|*
name|wfds
parameter_list|,
name|fd_set
modifier|*
name|efds
parameter_list|)
block|{
name|int
name|happened
init|=
literal|0
decl_stmt|;
if|if
condition|(
name|FD_ISSET
argument_list|(
name|fd
argument_list|,
name|rfds
argument_list|)
condition|)
block|{
name|int
name|r
decl_stmt|;
name|int
name|socket_errno
decl_stmt|;
if|#
directive|if
name|defined
name|__MACH__
operator|&&
name|defined
name|__APPLE__
comment|/* There is a bug in Mac OS X that causes it to ignore MSG_PEEK 	 for some kinds of descriptors.  Detect if this descriptor is a 	 connected socket, a server socket, or something else using a 	 0-byte recv, and use ioctl(2) to detect POLLHUP.  */
name|r
operator|=
name|recv
argument_list|(
name|fd
argument_list|,
name|NULL
argument_list|,
literal|0
argument_list|,
name|MSG_PEEK
argument_list|)
expr_stmt|;
name|socket_errno
operator|=
operator|(
name|r
operator|<
literal|0
operator|)
condition|?
name|errno
else|:
literal|0
expr_stmt|;
if|if
condition|(
name|r
operator|==
literal|0
operator|||
name|socket_errno
operator|==
name|ENOTSOCK
condition|)
name|ioctl
argument_list|(
name|fd
argument_list|,
name|FIONREAD
argument_list|,
operator|&
name|r
argument_list|)
expr_stmt|;
else|#
directive|else
name|char
name|data
index|[
literal|64
index|]
decl_stmt|;
name|r
operator|=
name|recv
argument_list|(
name|fd
argument_list|,
name|data
argument_list|,
sizeof|sizeof
argument_list|(
name|data
argument_list|)
argument_list|,
name|MSG_PEEK
argument_list|)
expr_stmt|;
name|socket_errno
operator|=
operator|(
name|r
operator|<
literal|0
operator|)
condition|?
name|errno
else|:
literal|0
expr_stmt|;
endif|#
directive|endif
if|if
condition|(
name|r
operator|==
literal|0
condition|)
name|happened
operator||=
name|POLLHUP
expr_stmt|;
comment|/* If the event happened on an unconnected server socket, 	 that's fine. */
elseif|else
if|if
condition|(
name|r
operator|>
literal|0
operator|||
operator|(
comment|/* (r == -1)&& */
name|socket_errno
operator|==
name|ENOTCONN
operator|)
condition|)
name|happened
operator||=
operator|(
name|POLLIN
operator||
name|POLLRDNORM
operator|)
operator|&
name|sought
expr_stmt|;
comment|/* Distinguish hung-up sockets from other errors.  */
elseif|else
if|if
condition|(
name|socket_errno
operator|==
name|ESHUTDOWN
operator|||
name|socket_errno
operator|==
name|ECONNRESET
operator|||
name|socket_errno
operator|==
name|ECONNABORTED
operator|||
name|socket_errno
operator|==
name|ENETRESET
condition|)
name|happened
operator||=
name|POLLHUP
expr_stmt|;
comment|/* some systems can't use recv() on non-socket, including HP NonStop */
elseif|else
if|if
condition|(
comment|/* (r == -1)&& */
name|socket_errno
operator|==
name|ENOTSOCK
condition|)
name|happened
operator||=
operator|(
name|POLLIN
operator||
name|POLLRDNORM
operator|)
operator|&
name|sought
expr_stmt|;
else|else
name|happened
operator||=
name|POLLERR
expr_stmt|;
block|}
if|if
condition|(
name|FD_ISSET
argument_list|(
name|fd
argument_list|,
name|wfds
argument_list|)
condition|)
name|happened
operator||=
operator|(
name|POLLOUT
operator||
name|POLLWRNORM
operator||
name|POLLWRBAND
operator|)
operator|&
name|sought
expr_stmt|;
if|if
condition|(
name|FD_ISSET
argument_list|(
name|fd
argument_list|,
name|efds
argument_list|)
condition|)
name|happened
operator||=
operator|(
name|POLLPRI
operator||
name|POLLRDBAND
operator|)
operator|&
name|sought
expr_stmt|;
return|return
name|happened
return|;
block|}
end_function
begin_endif
endif|#
directive|endif
end_endif
begin_comment
comment|/* !MinGW */
end_comment
begin_function
name|int
DECL|function|poll
name|poll
parameter_list|(
name|struct
name|pollfd
modifier|*
name|pfd
parameter_list|,
name|nfds_t
name|nfd
parameter_list|,
name|int
name|timeout
parameter_list|)
block|{
ifndef|#
directive|ifndef
name|WIN32_NATIVE
name|fd_set
name|rfds
decl_stmt|,
name|wfds
decl_stmt|,
name|efds
decl_stmt|;
name|struct
name|timeval
name|tv
decl_stmt|;
name|struct
name|timeval
modifier|*
name|ptv
decl_stmt|;
name|int
name|maxfd
decl_stmt|,
name|rc
decl_stmt|;
name|nfds_t
name|i
decl_stmt|;
ifdef|#
directive|ifdef
name|_SC_OPEN_MAX
specifier|static
name|int
name|sc_open_max
init|=
operator|-
literal|1
decl_stmt|;
if|if
condition|(
name|nfd
operator|<
literal|0
operator|||
operator|(
name|nfd
operator|>
name|sc_open_max
operator|&&
operator|(
name|sc_open_max
operator|!=
operator|-
literal|1
operator|||
name|nfd
operator|>
operator|(
name|sc_open_max
operator|=
name|sysconf
argument_list|(
name|_SC_OPEN_MAX
argument_list|)
operator|)
operator|)
operator|)
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
else|#
directive|else
comment|/* !_SC_OPEN_MAX */
ifdef|#
directive|ifdef
name|OPEN_MAX
if|if
condition|(
name|nfd
operator|<
literal|0
operator|||
name|nfd
operator|>
name|OPEN_MAX
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
endif|#
directive|endif
comment|/* OPEN_MAX -- else, no check is needed */
endif|#
directive|endif
comment|/* !_SC_OPEN_MAX */
comment|/* EFAULT is not necessary to implement, but let's do it in the      simplest case. */
if|if
condition|(
operator|!
name|pfd
operator|&&
name|nfd
condition|)
block|{
name|errno
operator|=
name|EFAULT
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
comment|/* convert timeout number into a timeval structure */
if|if
condition|(
name|timeout
operator|==
literal|0
condition|)
block|{
name|ptv
operator|=
operator|&
name|tv
expr_stmt|;
name|ptv
operator|->
name|tv_sec
operator|=
literal|0
expr_stmt|;
name|ptv
operator|->
name|tv_usec
operator|=
literal|0
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|timeout
operator|>
literal|0
condition|)
block|{
name|ptv
operator|=
operator|&
name|tv
expr_stmt|;
name|ptv
operator|->
name|tv_sec
operator|=
name|timeout
operator|/
literal|1000
expr_stmt|;
name|ptv
operator|->
name|tv_usec
operator|=
operator|(
name|timeout
operator|%
literal|1000
operator|)
operator|*
literal|1000
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|timeout
operator|==
name|INFTIM
condition|)
comment|/* wait forever */
name|ptv
operator|=
name|NULL
expr_stmt|;
else|else
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
comment|/* create fd sets and determine max fd */
name|maxfd
operator|=
operator|-
literal|1
expr_stmt|;
name|FD_ZERO
argument_list|(
operator|&
name|rfds
argument_list|)
expr_stmt|;
name|FD_ZERO
argument_list|(
operator|&
name|wfds
argument_list|)
expr_stmt|;
name|FD_ZERO
argument_list|(
operator|&
name|efds
argument_list|)
expr_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|nfd
condition|;
name|i
operator|++
control|)
block|{
if|if
condition|(
name|pfd
index|[
name|i
index|]
operator|.
name|fd
operator|<
literal|0
condition|)
continue|continue;
if|if
condition|(
name|pfd
index|[
name|i
index|]
operator|.
name|events
operator|&
operator|(
name|POLLIN
operator||
name|POLLRDNORM
operator|)
condition|)
name|FD_SET
argument_list|(
name|pfd
index|[
name|i
index|]
operator|.
name|fd
argument_list|,
operator|&
name|rfds
argument_list|)
expr_stmt|;
comment|/* see select(2): "the only exceptional condition detectable 	 is out-of-band data received on a socket", hence we push 	 POLLWRBAND events onto wfds instead of efds. */
if|if
condition|(
name|pfd
index|[
name|i
index|]
operator|.
name|events
operator|&
operator|(
name|POLLOUT
operator||
name|POLLWRNORM
operator||
name|POLLWRBAND
operator|)
condition|)
name|FD_SET
argument_list|(
name|pfd
index|[
name|i
index|]
operator|.
name|fd
argument_list|,
operator|&
name|wfds
argument_list|)
expr_stmt|;
if|if
condition|(
name|pfd
index|[
name|i
index|]
operator|.
name|events
operator|&
operator|(
name|POLLPRI
operator||
name|POLLRDBAND
operator|)
condition|)
name|FD_SET
argument_list|(
name|pfd
index|[
name|i
index|]
operator|.
name|fd
argument_list|,
operator|&
name|efds
argument_list|)
expr_stmt|;
if|if
condition|(
name|pfd
index|[
name|i
index|]
operator|.
name|fd
operator|>=
name|maxfd
operator|&&
operator|(
name|pfd
index|[
name|i
index|]
operator|.
name|events
operator|&
operator|(
name|POLLIN
operator||
name|POLLOUT
operator||
name|POLLPRI
operator||
name|POLLRDNORM
operator||
name|POLLRDBAND
operator||
name|POLLWRNORM
operator||
name|POLLWRBAND
operator|)
operator|)
condition|)
block|{
name|maxfd
operator|=
name|pfd
index|[
name|i
index|]
operator|.
name|fd
expr_stmt|;
if|if
condition|(
name|maxfd
operator|>
name|FD_SETSIZE
condition|)
block|{
name|errno
operator|=
name|EOVERFLOW
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
block|}
block|}
comment|/* examine fd sets */
name|rc
operator|=
name|select
argument_list|(
name|maxfd
operator|+
literal|1
argument_list|,
operator|&
name|rfds
argument_list|,
operator|&
name|wfds
argument_list|,
operator|&
name|efds
argument_list|,
name|ptv
argument_list|)
expr_stmt|;
if|if
condition|(
name|rc
operator|<
literal|0
condition|)
return|return
name|rc
return|;
comment|/* establish results */
name|rc
operator|=
literal|0
expr_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|nfd
condition|;
name|i
operator|++
control|)
if|if
condition|(
name|pfd
index|[
name|i
index|]
operator|.
name|fd
operator|<
literal|0
condition|)
name|pfd
index|[
name|i
index|]
operator|.
name|revents
operator|=
literal|0
expr_stmt|;
else|else
block|{
name|int
name|happened
init|=
name|compute_revents
argument_list|(
name|pfd
index|[
name|i
index|]
operator|.
name|fd
argument_list|,
name|pfd
index|[
name|i
index|]
operator|.
name|events
argument_list|,
operator|&
name|rfds
argument_list|,
operator|&
name|wfds
argument_list|,
operator|&
name|efds
argument_list|)
decl_stmt|;
if|if
condition|(
name|happened
condition|)
block|{
name|pfd
index|[
name|i
index|]
operator|.
name|revents
operator|=
name|happened
expr_stmt|;
name|rc
operator|++
expr_stmt|;
block|}
block|}
return|return
name|rc
return|;
else|#
directive|else
specifier|static
name|struct
name|timeval
name|tv0
decl_stmt|;
specifier|static
name|HANDLE
name|hEvent
decl_stmt|;
name|WSANETWORKEVENTS
name|ev
decl_stmt|;
name|HANDLE
name|h
decl_stmt|,
name|handle_array
index|[
name|FD_SETSIZE
operator|+
literal|2
index|]
decl_stmt|;
name|DWORD
name|ret
decl_stmt|,
name|wait_timeout
decl_stmt|,
name|nhandles
decl_stmt|;
name|fd_set
name|rfds
decl_stmt|,
name|wfds
decl_stmt|,
name|xfds
decl_stmt|;
name|BOOL
name|poll_again
decl_stmt|;
name|MSG
name|msg
decl_stmt|;
name|int
name|rc
init|=
literal|0
decl_stmt|;
name|nfds_t
name|i
decl_stmt|;
if|if
condition|(
name|nfd
operator|<
literal|0
operator|||
name|timeout
operator|<
operator|-
literal|1
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
if|if
condition|(
operator|!
name|hEvent
condition|)
name|hEvent
operator|=
name|CreateEvent
argument_list|(
name|NULL
argument_list|,
name|FALSE
argument_list|,
name|FALSE
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|restart
label|:
name|handle_array
index|[
literal|0
index|]
operator|=
name|hEvent
expr_stmt|;
name|nhandles
operator|=
literal|1
expr_stmt|;
name|FD_ZERO
argument_list|(
operator|&
name|rfds
argument_list|)
expr_stmt|;
name|FD_ZERO
argument_list|(
operator|&
name|wfds
argument_list|)
expr_stmt|;
name|FD_ZERO
argument_list|(
operator|&
name|xfds
argument_list|)
expr_stmt|;
comment|/* Classify socket handles and create fd sets. */
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|nfd
condition|;
name|i
operator|++
control|)
block|{
name|int
name|sought
init|=
name|pfd
index|[
name|i
index|]
operator|.
name|events
decl_stmt|;
name|pfd
index|[
name|i
index|]
operator|.
name|revents
operator|=
literal|0
expr_stmt|;
if|if
condition|(
name|pfd
index|[
name|i
index|]
operator|.
name|fd
operator|<
literal|0
condition|)
continue|continue;
if|if
condition|(
operator|!
operator|(
name|sought
operator|&
operator|(
name|POLLIN
operator||
name|POLLRDNORM
operator||
name|POLLOUT
operator||
name|POLLWRNORM
operator||
name|POLLWRBAND
operator||
name|POLLPRI
operator||
name|POLLRDBAND
operator|)
operator|)
condition|)
continue|continue;
name|h
operator|=
operator|(
name|HANDLE
operator|)
name|_get_osfhandle
argument_list|(
name|pfd
index|[
name|i
index|]
operator|.
name|fd
argument_list|)
expr_stmt|;
name|assert
argument_list|(
name|h
operator|!=
name|NULL
argument_list|)
expr_stmt|;
if|if
condition|(
name|IsSocketHandle
argument_list|(
name|h
argument_list|)
condition|)
block|{
name|int
name|requested
init|=
name|FD_CLOSE
decl_stmt|;
comment|/* see above; socket handles are mapped onto select.  */
if|if
condition|(
name|sought
operator|&
operator|(
name|POLLIN
operator||
name|POLLRDNORM
operator|)
condition|)
block|{
name|requested
operator||=
name|FD_READ
operator||
name|FD_ACCEPT
expr_stmt|;
name|FD_SET
argument_list|(
operator|(
name|SOCKET
operator|)
name|h
argument_list|,
operator|&
name|rfds
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|sought
operator|&
operator|(
name|POLLOUT
operator||
name|POLLWRNORM
operator||
name|POLLWRBAND
operator|)
condition|)
block|{
name|requested
operator||=
name|FD_WRITE
operator||
name|FD_CONNECT
expr_stmt|;
name|FD_SET
argument_list|(
operator|(
name|SOCKET
operator|)
name|h
argument_list|,
operator|&
name|wfds
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|sought
operator|&
operator|(
name|POLLPRI
operator||
name|POLLRDBAND
operator|)
condition|)
block|{
name|requested
operator||=
name|FD_OOB
expr_stmt|;
name|FD_SET
argument_list|(
operator|(
name|SOCKET
operator|)
name|h
argument_list|,
operator|&
name|xfds
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|requested
condition|)
name|WSAEventSelect
argument_list|(
operator|(
name|SOCKET
operator|)
name|h
argument_list|,
name|hEvent
argument_list|,
name|requested
argument_list|)
expr_stmt|;
block|}
else|else
block|{
comment|/* Poll now.  If we get an event, do not poll again.  Also, 	     screen buffer handles are waitable, and they'll block until 	     a character is available.  win32_compute_revents eliminates 	     bits for the "wrong" direction. */
name|pfd
index|[
name|i
index|]
operator|.
name|revents
operator|=
name|win32_compute_revents
argument_list|(
name|h
argument_list|,
operator|&
name|sought
argument_list|)
expr_stmt|;
if|if
condition|(
name|sought
condition|)
name|handle_array
index|[
name|nhandles
operator|++
index|]
operator|=
name|h
expr_stmt|;
if|if
condition|(
name|pfd
index|[
name|i
index|]
operator|.
name|revents
condition|)
name|timeout
operator|=
literal|0
expr_stmt|;
block|}
block|}
if|if
condition|(
name|select
argument_list|(
literal|0
argument_list|,
operator|&
name|rfds
argument_list|,
operator|&
name|wfds
argument_list|,
operator|&
name|xfds
argument_list|,
operator|&
name|tv0
argument_list|)
operator|>
literal|0
condition|)
block|{
comment|/* Do MsgWaitForMultipleObjects anyway to dispatch messages, but 	 no need to call select again.  */
name|poll_again
operator|=
name|FALSE
expr_stmt|;
name|wait_timeout
operator|=
literal|0
expr_stmt|;
block|}
else|else
block|{
name|poll_again
operator|=
name|TRUE
expr_stmt|;
if|if
condition|(
name|timeout
operator|==
name|INFTIM
condition|)
name|wait_timeout
operator|=
name|INFINITE
expr_stmt|;
else|else
name|wait_timeout
operator|=
name|timeout
expr_stmt|;
block|}
for|for
control|(
init|;
condition|;
control|)
block|{
name|ret
operator|=
name|MsgWaitForMultipleObjects
argument_list|(
name|nhandles
argument_list|,
name|handle_array
argument_list|,
name|FALSE
argument_list|,
name|wait_timeout
argument_list|,
name|QS_ALLINPUT
argument_list|)
expr_stmt|;
if|if
condition|(
name|ret
operator|==
name|WAIT_OBJECT_0
operator|+
name|nhandles
condition|)
block|{
comment|/* new input of some other kind */
name|BOOL
name|bRet
decl_stmt|;
while|while
condition|(
operator|(
name|bRet
operator|=
name|PeekMessage
argument_list|(
operator|&
name|msg
argument_list|,
name|NULL
argument_list|,
literal|0
argument_list|,
literal|0
argument_list|,
name|PM_REMOVE
argument_list|)
operator|)
operator|!=
literal|0
condition|)
block|{
name|TranslateMessage
argument_list|(
operator|&
name|msg
argument_list|)
expr_stmt|;
name|DispatchMessage
argument_list|(
operator|&
name|msg
argument_list|)
expr_stmt|;
block|}
block|}
else|else
break|break;
block|}
if|if
condition|(
name|poll_again
condition|)
name|select
argument_list|(
literal|0
argument_list|,
operator|&
name|rfds
argument_list|,
operator|&
name|wfds
argument_list|,
operator|&
name|xfds
argument_list|,
operator|&
name|tv0
argument_list|)
expr_stmt|;
comment|/* Place a sentinel at the end of the array.  */
name|handle_array
index|[
name|nhandles
index|]
operator|=
name|NULL
expr_stmt|;
name|nhandles
operator|=
literal|1
expr_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|nfd
condition|;
name|i
operator|++
control|)
block|{
name|int
name|happened
decl_stmt|;
if|if
condition|(
name|pfd
index|[
name|i
index|]
operator|.
name|fd
operator|<
literal|0
condition|)
continue|continue;
if|if
condition|(
operator|!
operator|(
name|pfd
index|[
name|i
index|]
operator|.
name|events
operator|&
operator|(
name|POLLIN
operator||
name|POLLRDNORM
operator||
name|POLLOUT
operator||
name|POLLWRNORM
operator||
name|POLLWRBAND
operator|)
operator|)
condition|)
continue|continue;
name|h
operator|=
operator|(
name|HANDLE
operator|)
name|_get_osfhandle
argument_list|(
name|pfd
index|[
name|i
index|]
operator|.
name|fd
argument_list|)
expr_stmt|;
if|if
condition|(
name|h
operator|!=
name|handle_array
index|[
name|nhandles
index|]
condition|)
block|{
comment|/* It's a socket.  */
name|WSAEnumNetworkEvents
argument_list|(
operator|(
name|SOCKET
operator|)
name|h
argument_list|,
name|NULL
argument_list|,
operator|&
name|ev
argument_list|)
expr_stmt|;
name|WSAEventSelect
argument_list|(
operator|(
name|SOCKET
operator|)
name|h
argument_list|,
name|NULL
argument_list|,
literal|0
argument_list|)
expr_stmt|;
comment|/* If we're lucky, WSAEnumNetworkEvents already provided a way 	     to distinguish FD_READ and FD_ACCEPT; this saves a recv later.  */
if|if
condition|(
name|FD_ISSET
argument_list|(
operator|(
name|SOCKET
operator|)
name|h
argument_list|,
operator|&
name|rfds
argument_list|)
operator|&&
operator|!
operator|(
name|ev
operator|.
name|lNetworkEvents
operator|&
operator|(
name|FD_READ
operator||
name|FD_ACCEPT
operator|)
operator|)
condition|)
name|ev
operator|.
name|lNetworkEvents
operator||=
name|FD_READ
operator||
name|FD_ACCEPT
expr_stmt|;
if|if
condition|(
name|FD_ISSET
argument_list|(
operator|(
name|SOCKET
operator|)
name|h
argument_list|,
operator|&
name|wfds
argument_list|)
condition|)
name|ev
operator|.
name|lNetworkEvents
operator||=
name|FD_WRITE
operator||
name|FD_CONNECT
expr_stmt|;
if|if
condition|(
name|FD_ISSET
argument_list|(
operator|(
name|SOCKET
operator|)
name|h
argument_list|,
operator|&
name|xfds
argument_list|)
condition|)
name|ev
operator|.
name|lNetworkEvents
operator||=
name|FD_OOB
expr_stmt|;
name|happened
operator|=
name|win32_compute_revents_socket
argument_list|(
operator|(
name|SOCKET
operator|)
name|h
argument_list|,
name|pfd
index|[
name|i
index|]
operator|.
name|events
argument_list|,
name|ev
operator|.
name|lNetworkEvents
argument_list|)
expr_stmt|;
block|}
else|else
block|{
comment|/* Not a socket.  */
name|int
name|sought
init|=
name|pfd
index|[
name|i
index|]
operator|.
name|events
decl_stmt|;
name|happened
operator|=
name|win32_compute_revents
argument_list|(
name|h
argument_list|,
operator|&
name|sought
argument_list|)
expr_stmt|;
name|nhandles
operator|++
expr_stmt|;
block|}
if|if
condition|(
operator|(
name|pfd
index|[
name|i
index|]
operator|.
name|revents
operator||=
name|happened
operator|)
operator|!=
literal|0
condition|)
name|rc
operator|++
expr_stmt|;
block|}
if|if
condition|(
operator|!
name|rc
operator|&&
name|timeout
operator|==
name|INFTIM
condition|)
block|{
name|SwitchToThread
argument_list|()
expr_stmt|;
goto|goto
name|restart
goto|;
block|}
return|return
name|rc
return|;
endif|#
directive|endif
block|}
end_function
end_unit
