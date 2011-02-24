begin_unit
begin_comment
comment|/*  * GIT - The information manager from hell  *  * Copyright (C) 2000-2002 Michael R. Elkins<me@mutt.org>  * Copyright (C) 2002-2004 Oswald Buddenhagen<ossi@users.sf.net>  * Copyright (C) 2004 Theodore Y. Ts'o<tytso@mit.edu>  * Copyright (C) 2006 Mike McCormack  * Copyright (C) 2006 Christian Couder  *  *  This program is free software; you can redistribute it and/or modify  *  it under the terms of the GNU General Public License as published by  *  the Free Software Foundation; either version 2 of the License, or  *  (at your option) any later version.  *  *  This program is distributed in the hope that it will be useful,  *  but WITHOUT ANY WARRANTY; without even the implied warranty of  *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the  *  GNU General Public License for more details.  *  *  You should have received a copy of the GNU General Public License  *  along with this program; if not, write to the Free Software  *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA  */
end_comment
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"quote.h"
end_include
begin_comment
comment|/* Get a trace file descriptor from "key" env variable. */
end_comment
begin_function
DECL|function|get_trace_fd
specifier|static
name|int
name|get_trace_fd
parameter_list|(
specifier|const
name|char
modifier|*
name|key
parameter_list|,
name|int
modifier|*
name|need_close
parameter_list|)
block|{
name|char
modifier|*
name|trace
init|=
name|getenv
argument_list|(
name|key
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
name|trace
operator|||
operator|!
name|strcmp
argument_list|(
name|trace
argument_list|,
literal|""
argument_list|)
operator|||
operator|!
name|strcmp
argument_list|(
name|trace
argument_list|,
literal|"0"
argument_list|)
operator|||
operator|!
name|strcasecmp
argument_list|(
name|trace
argument_list|,
literal|"false"
argument_list|)
condition|)
return|return
literal|0
return|;
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|trace
argument_list|,
literal|"1"
argument_list|)
operator|||
operator|!
name|strcasecmp
argument_list|(
name|trace
argument_list|,
literal|"true"
argument_list|)
condition|)
return|return
name|STDERR_FILENO
return|;
if|if
condition|(
name|strlen
argument_list|(
name|trace
argument_list|)
operator|==
literal|1
operator|&&
name|isdigit
argument_list|(
operator|*
name|trace
argument_list|)
condition|)
return|return
name|atoi
argument_list|(
name|trace
argument_list|)
return|;
if|if
condition|(
name|is_absolute_path
argument_list|(
name|trace
argument_list|)
condition|)
block|{
name|int
name|fd
init|=
name|open
argument_list|(
name|trace
argument_list|,
name|O_WRONLY
operator||
name|O_APPEND
operator||
name|O_CREAT
argument_list|,
literal|0666
argument_list|)
decl_stmt|;
if|if
condition|(
name|fd
operator|==
operator|-
literal|1
condition|)
block|{
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"Could not open '%s' for tracing: %s\n"
literal|"Defaulting to tracing on stderr...\n"
argument_list|,
name|trace
argument_list|,
name|strerror
argument_list|(
name|errno
argument_list|)
argument_list|)
expr_stmt|;
return|return
name|STDERR_FILENO
return|;
block|}
operator|*
name|need_close
operator|=
literal|1
expr_stmt|;
return|return
name|fd
return|;
block|}
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"What does '%s' for %s mean?\n"
argument_list|,
name|trace
argument_list|,
name|key
argument_list|)
expr_stmt|;
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"If you want to trace into a file, "
literal|"then please set %s to an absolute pathname "
literal|"(starting with /).\n"
argument_list|,
name|key
argument_list|)
expr_stmt|;
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"Defaulting to tracing on stderr...\n"
argument_list|)
expr_stmt|;
return|return
name|STDERR_FILENO
return|;
block|}
end_function
begin_decl_stmt
DECL|variable|err_msg
specifier|static
specifier|const
name|char
name|err_msg
index|[]
init|=
literal|"Could not trace into fd given by "
literal|"GIT_TRACE environment variable"
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|trace_vprintf
name|void
name|trace_vprintf
parameter_list|(
specifier|const
name|char
modifier|*
name|key
parameter_list|,
specifier|const
name|char
modifier|*
name|fmt
parameter_list|,
name|va_list
name|ap
parameter_list|)
block|{
name|struct
name|strbuf
name|buf
init|=
name|STRBUF_INIT
decl_stmt|;
name|int
name|fd
decl_stmt|,
name|need_close
init|=
literal|0
decl_stmt|;
name|fd
operator|=
name|get_trace_fd
argument_list|(
name|key
argument_list|,
operator|&
name|need_close
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|fd
condition|)
return|return;
name|set_try_to_free_routine
argument_list|(
name|NULL
argument_list|)
expr_stmt|;
comment|/* is never reset */
name|strbuf_vaddf
argument_list|(
operator|&
name|buf
argument_list|,
name|fmt
argument_list|,
name|ap
argument_list|)
expr_stmt|;
name|write_or_whine_pipe
argument_list|(
name|fd
argument_list|,
name|buf
operator|.
name|buf
argument_list|,
name|buf
operator|.
name|len
argument_list|,
name|err_msg
argument_list|)
expr_stmt|;
name|strbuf_release
argument_list|(
operator|&
name|buf
argument_list|)
expr_stmt|;
if|if
condition|(
name|need_close
condition|)
name|close
argument_list|(
name|fd
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|trace_printf_key
name|void
name|trace_printf_key
parameter_list|(
specifier|const
name|char
modifier|*
name|key
parameter_list|,
specifier|const
name|char
modifier|*
name|fmt
parameter_list|,
modifier|...
parameter_list|)
block|{
name|va_list
name|ap
decl_stmt|;
name|va_start
argument_list|(
name|ap
argument_list|,
name|fmt
argument_list|)
expr_stmt|;
name|trace_vprintf
argument_list|(
name|key
argument_list|,
name|fmt
argument_list|,
name|ap
argument_list|)
expr_stmt|;
name|va_end
argument_list|(
name|ap
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|trace_printf
name|void
name|trace_printf
parameter_list|(
specifier|const
name|char
modifier|*
name|fmt
parameter_list|,
modifier|...
parameter_list|)
block|{
name|va_list
name|ap
decl_stmt|;
name|va_start
argument_list|(
name|ap
argument_list|,
name|fmt
argument_list|)
expr_stmt|;
name|trace_vprintf
argument_list|(
literal|"GIT_TRACE"
argument_list|,
name|fmt
argument_list|,
name|ap
argument_list|)
expr_stmt|;
name|va_end
argument_list|(
name|ap
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|trace_argv_printf
name|void
name|trace_argv_printf
parameter_list|(
specifier|const
name|char
modifier|*
modifier|*
name|argv
parameter_list|,
specifier|const
name|char
modifier|*
name|fmt
parameter_list|,
modifier|...
parameter_list|)
block|{
name|struct
name|strbuf
name|buf
init|=
name|STRBUF_INIT
decl_stmt|;
name|va_list
name|ap
decl_stmt|;
name|int
name|fd
decl_stmt|,
name|need_close
init|=
literal|0
decl_stmt|;
name|fd
operator|=
name|get_trace_fd
argument_list|(
literal|"GIT_TRACE"
argument_list|,
operator|&
name|need_close
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|fd
condition|)
return|return;
name|set_try_to_free_routine
argument_list|(
name|NULL
argument_list|)
expr_stmt|;
comment|/* is never reset */
name|va_start
argument_list|(
name|ap
argument_list|,
name|fmt
argument_list|)
expr_stmt|;
name|strbuf_vaddf
argument_list|(
operator|&
name|buf
argument_list|,
name|fmt
argument_list|,
name|ap
argument_list|)
expr_stmt|;
name|va_end
argument_list|(
name|ap
argument_list|)
expr_stmt|;
name|sq_quote_argv
argument_list|(
operator|&
name|buf
argument_list|,
name|argv
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|strbuf_addch
argument_list|(
operator|&
name|buf
argument_list|,
literal|'\n'
argument_list|)
expr_stmt|;
name|write_or_whine_pipe
argument_list|(
name|fd
argument_list|,
name|buf
operator|.
name|buf
argument_list|,
name|buf
operator|.
name|len
argument_list|,
name|err_msg
argument_list|)
expr_stmt|;
name|strbuf_release
argument_list|(
operator|&
name|buf
argument_list|)
expr_stmt|;
if|if
condition|(
name|need_close
condition|)
name|close
argument_list|(
name|fd
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|quote_crnl
specifier|static
specifier|const
name|char
modifier|*
name|quote_crnl
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|)
block|{
specifier|static
name|char
name|new_path
index|[
name|PATH_MAX
index|]
decl_stmt|;
specifier|const
name|char
modifier|*
name|p2
init|=
name|path
decl_stmt|;
name|char
modifier|*
name|p1
init|=
name|new_path
decl_stmt|;
if|if
condition|(
operator|!
name|path
condition|)
return|return
name|NULL
return|;
while|while
condition|(
operator|*
name|p2
condition|)
block|{
switch|switch
condition|(
operator|*
name|p2
condition|)
block|{
case|case
literal|'\\'
case|:
operator|*
name|p1
operator|++
operator|=
literal|'\\'
expr_stmt|;
operator|*
name|p1
operator|++
operator|=
literal|'\\'
expr_stmt|;
break|break;
case|case
literal|'\n'
case|:
operator|*
name|p1
operator|++
operator|=
literal|'\\'
expr_stmt|;
operator|*
name|p1
operator|++
operator|=
literal|'n'
expr_stmt|;
break|break;
case|case
literal|'\r'
case|:
operator|*
name|p1
operator|++
operator|=
literal|'\\'
expr_stmt|;
operator|*
name|p1
operator|++
operator|=
literal|'r'
expr_stmt|;
break|break;
default|default:
operator|*
name|p1
operator|++
operator|=
operator|*
name|p2
expr_stmt|;
block|}
name|p2
operator|++
expr_stmt|;
block|}
operator|*
name|p1
operator|=
literal|'\0'
expr_stmt|;
return|return
name|new_path
return|;
block|}
end_function
begin_comment
comment|/* FIXME: move prefix to startup_info struct and get rid of this arg */
end_comment
begin_function
DECL|function|trace_repo_setup
name|void
name|trace_repo_setup
parameter_list|(
specifier|const
name|char
modifier|*
name|prefix
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|git_work_tree
decl_stmt|;
name|char
name|cwd
index|[
name|PATH_MAX
index|]
decl_stmt|;
name|char
modifier|*
name|trace
init|=
name|getenv
argument_list|(
literal|"GIT_TRACE"
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
name|trace
operator|||
operator|!
name|strcmp
argument_list|(
name|trace
argument_list|,
literal|""
argument_list|)
operator|||
operator|!
name|strcmp
argument_list|(
name|trace
argument_list|,
literal|"0"
argument_list|)
operator|||
operator|!
name|strcasecmp
argument_list|(
name|trace
argument_list|,
literal|"false"
argument_list|)
condition|)
return|return;
if|if
condition|(
operator|!
name|getcwd
argument_list|(
name|cwd
argument_list|,
name|PATH_MAX
argument_list|)
condition|)
name|die
argument_list|(
literal|"Unable to get current working directory"
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
operator|(
name|git_work_tree
operator|=
name|get_git_work_tree
argument_list|()
operator|)
condition|)
name|git_work_tree
operator|=
literal|"(null)"
expr_stmt|;
if|if
condition|(
operator|!
name|prefix
condition|)
name|prefix
operator|=
literal|"(null)"
expr_stmt|;
name|trace_printf
argument_list|(
literal|"setup: git_dir: %s\n"
argument_list|,
name|quote_crnl
argument_list|(
name|get_git_dir
argument_list|()
argument_list|)
argument_list|)
expr_stmt|;
name|trace_printf
argument_list|(
literal|"setup: worktree: %s\n"
argument_list|,
name|quote_crnl
argument_list|(
name|git_work_tree
argument_list|)
argument_list|)
expr_stmt|;
name|trace_printf
argument_list|(
literal|"setup: cwd: %s\n"
argument_list|,
name|quote_crnl
argument_list|(
name|cwd
argument_list|)
argument_list|)
expr_stmt|;
name|trace_printf
argument_list|(
literal|"setup: prefix: %s\n"
argument_list|,
name|quote_crnl
argument_list|(
name|prefix
argument_list|)
argument_list|)
expr_stmt|;
block|}
end_function
end_unit
