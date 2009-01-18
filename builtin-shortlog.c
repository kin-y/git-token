begin_unit
begin_include
include|#
directive|include
file|"builtin.h"
end_include
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"commit.h"
end_include
begin_include
include|#
directive|include
file|"diff.h"
end_include
begin_include
include|#
directive|include
file|"string-list.h"
end_include
begin_include
include|#
directive|include
file|"revision.h"
end_include
begin_include
include|#
directive|include
file|"utf8.h"
end_include
begin_include
include|#
directive|include
file|"mailmap.h"
end_include
begin_include
include|#
directive|include
file|"shortlog.h"
end_include
begin_include
include|#
directive|include
file|"parse-options.h"
end_include
begin_decl_stmt
DECL|variable|shortlog_usage
specifier|static
name|char
specifier|const
modifier|*
specifier|const
name|shortlog_usage
index|[]
init|=
block|{
literal|"git shortlog [-n] [-s] [-e] [-w] [rev-opts] [--] [<commit-id>... ]"
block|,
literal|""
block|,
literal|"[rev-opts] are documented in git-rev-list(1)"
block|,
name|NULL
block|}
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|compare_by_number
specifier|static
name|int
name|compare_by_number
parameter_list|(
specifier|const
name|void
modifier|*
name|a1
parameter_list|,
specifier|const
name|void
modifier|*
name|a2
parameter_list|)
block|{
specifier|const
name|struct
name|string_list_item
modifier|*
name|i1
init|=
name|a1
decl_stmt|,
modifier|*
name|i2
init|=
name|a2
decl_stmt|;
specifier|const
name|struct
name|string_list
modifier|*
name|l1
init|=
name|i1
operator|->
name|util
decl_stmt|,
modifier|*
name|l2
init|=
name|i2
operator|->
name|util
decl_stmt|;
if|if
condition|(
name|l1
operator|->
name|nr
operator|<
name|l2
operator|->
name|nr
condition|)
return|return
literal|1
return|;
elseif|else
if|if
condition|(
name|l1
operator|->
name|nr
operator|==
name|l2
operator|->
name|nr
condition|)
return|return
literal|0
return|;
else|else
return|return
operator|-
literal|1
return|;
block|}
end_function
begin_function_decl
specifier|const
name|char
modifier|*
name|format_subject
parameter_list|(
name|struct
name|strbuf
modifier|*
name|sb
parameter_list|,
specifier|const
name|char
modifier|*
name|msg
parameter_list|,
specifier|const
name|char
modifier|*
name|line_separator
parameter_list|)
function_decl|;
end_function_decl
begin_function
DECL|function|insert_one_record
specifier|static
name|void
name|insert_one_record
parameter_list|(
name|struct
name|shortlog
modifier|*
name|log
parameter_list|,
specifier|const
name|char
modifier|*
name|author
parameter_list|,
specifier|const
name|char
modifier|*
name|oneline
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|dot3
init|=
name|log
operator|->
name|common_repo_prefix
decl_stmt|;
name|char
modifier|*
name|buffer
decl_stmt|,
modifier|*
name|p
decl_stmt|;
name|struct
name|string_list_item
modifier|*
name|item
decl_stmt|;
name|char
name|namebuf
index|[
literal|1024
index|]
decl_stmt|;
name|size_t
name|len
decl_stmt|;
specifier|const
name|char
modifier|*
name|eol
decl_stmt|;
specifier|const
name|char
modifier|*
name|boemail
decl_stmt|,
modifier|*
name|eoemail
decl_stmt|;
name|struct
name|strbuf
name|subject
init|=
name|STRBUF_INIT
decl_stmt|;
name|boemail
operator|=
name|strchr
argument_list|(
name|author
argument_list|,
literal|'<'
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|boemail
condition|)
return|return;
name|eoemail
operator|=
name|strchr
argument_list|(
name|boemail
argument_list|,
literal|'>'
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|eoemail
condition|)
return|return;
if|if
condition|(
operator|!
name|map_email
argument_list|(
operator|&
name|log
operator|->
name|mailmap
argument_list|,
name|boemail
operator|+
literal|1
argument_list|,
name|namebuf
argument_list|,
sizeof|sizeof
argument_list|(
name|namebuf
argument_list|)
argument_list|)
condition|)
block|{
while|while
condition|(
name|author
operator|<
name|boemail
operator|&&
name|isspace
argument_list|(
operator|*
name|author
argument_list|)
condition|)
name|author
operator|++
expr_stmt|;
for|for
control|(
name|len
operator|=
literal|0
init|;
name|len
operator|<
sizeof|sizeof
argument_list|(
name|namebuf
argument_list|)
operator|-
literal|1
operator|&&
name|author
operator|+
name|len
operator|<
name|boemail
condition|;
name|len
operator|++
control|)
name|namebuf
index|[
name|len
index|]
operator|=
name|author
index|[
name|len
index|]
expr_stmt|;
while|while
condition|(
literal|0
operator|<
name|len
operator|&&
name|isspace
argument_list|(
name|namebuf
index|[
name|len
operator|-
literal|1
index|]
argument_list|)
condition|)
name|len
operator|--
expr_stmt|;
name|namebuf
index|[
name|len
index|]
operator|=
literal|'\0'
expr_stmt|;
block|}
else|else
name|len
operator|=
name|strlen
argument_list|(
name|namebuf
argument_list|)
expr_stmt|;
if|if
condition|(
name|log
operator|->
name|email
condition|)
block|{
name|size_t
name|room
init|=
sizeof|sizeof
argument_list|(
name|namebuf
argument_list|)
operator|-
name|len
operator|-
literal|1
decl_stmt|;
name|int
name|maillen
init|=
name|eoemail
operator|-
name|boemail
operator|+
literal|1
decl_stmt|;
name|snprintf
argument_list|(
name|namebuf
operator|+
name|len
argument_list|,
name|room
argument_list|,
literal|" %.*s"
argument_list|,
name|maillen
argument_list|,
name|boemail
argument_list|)
expr_stmt|;
block|}
name|item
operator|=
name|string_list_insert
argument_list|(
name|namebuf
argument_list|,
operator|&
name|log
operator|->
name|list
argument_list|)
expr_stmt|;
if|if
condition|(
name|item
operator|->
name|util
operator|==
name|NULL
condition|)
name|item
operator|->
name|util
operator|=
name|xcalloc
argument_list|(
literal|1
argument_list|,
sizeof|sizeof
argument_list|(
expr|struct
name|string_list
argument_list|)
argument_list|)
expr_stmt|;
comment|/* Skip any leading whitespace, including any blank lines. */
while|while
condition|(
operator|*
name|oneline
operator|&&
name|isspace
argument_list|(
operator|*
name|oneline
argument_list|)
condition|)
name|oneline
operator|++
expr_stmt|;
name|eol
operator|=
name|strchr
argument_list|(
name|oneline
argument_list|,
literal|'\n'
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|eol
condition|)
name|eol
operator|=
name|oneline
operator|+
name|strlen
argument_list|(
name|oneline
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|prefixcmp
argument_list|(
name|oneline
argument_list|,
literal|"[PATCH"
argument_list|)
condition|)
block|{
name|char
modifier|*
name|eob
init|=
name|strchr
argument_list|(
name|oneline
argument_list|,
literal|']'
argument_list|)
decl_stmt|;
if|if
condition|(
name|eob
operator|&&
operator|(
operator|!
name|eol
operator|||
name|eob
operator|<
name|eol
operator|)
condition|)
name|oneline
operator|=
name|eob
operator|+
literal|1
expr_stmt|;
block|}
while|while
condition|(
operator|*
name|oneline
operator|&&
name|isspace
argument_list|(
operator|*
name|oneline
argument_list|)
operator|&&
operator|*
name|oneline
operator|!=
literal|'\n'
condition|)
name|oneline
operator|++
expr_stmt|;
name|len
operator|=
name|eol
operator|-
name|oneline
expr_stmt|;
name|format_subject
argument_list|(
operator|&
name|subject
argument_list|,
name|oneline
argument_list|,
literal|" "
argument_list|)
expr_stmt|;
name|buffer
operator|=
name|strbuf_detach
argument_list|(
operator|&
name|subject
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
if|if
condition|(
name|dot3
condition|)
block|{
name|int
name|dot3len
init|=
name|strlen
argument_list|(
name|dot3
argument_list|)
decl_stmt|;
if|if
condition|(
name|dot3len
operator|>
literal|5
condition|)
block|{
while|while
condition|(
operator|(
name|p
operator|=
name|strstr
argument_list|(
name|buffer
argument_list|,
name|dot3
argument_list|)
operator|)
operator|!=
name|NULL
condition|)
block|{
name|int
name|taillen
init|=
name|strlen
argument_list|(
name|p
argument_list|)
operator|-
name|dot3len
decl_stmt|;
name|memcpy
argument_list|(
name|p
argument_list|,
literal|"/.../"
argument_list|,
literal|5
argument_list|)
expr_stmt|;
name|memmove
argument_list|(
name|p
operator|+
literal|5
argument_list|,
name|p
operator|+
name|dot3len
argument_list|,
name|taillen
operator|+
literal|1
argument_list|)
expr_stmt|;
block|}
block|}
block|}
name|string_list_append
argument_list|(
name|buffer
argument_list|,
name|item
operator|->
name|util
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|read_from_stdin
specifier|static
name|void
name|read_from_stdin
parameter_list|(
name|struct
name|shortlog
modifier|*
name|log
parameter_list|)
block|{
name|char
name|author
index|[
literal|1024
index|]
decl_stmt|,
name|oneline
index|[
literal|1024
index|]
decl_stmt|;
while|while
condition|(
name|fgets
argument_list|(
name|author
argument_list|,
sizeof|sizeof
argument_list|(
name|author
argument_list|)
argument_list|,
name|stdin
argument_list|)
operator|!=
name|NULL
condition|)
block|{
if|if
condition|(
operator|!
operator|(
name|author
index|[
literal|0
index|]
operator|==
literal|'A'
operator|||
name|author
index|[
literal|0
index|]
operator|==
literal|'a'
operator|)
operator|||
name|prefixcmp
argument_list|(
name|author
operator|+
literal|1
argument_list|,
literal|"uthor: "
argument_list|)
condition|)
continue|continue;
while|while
condition|(
name|fgets
argument_list|(
name|oneline
argument_list|,
sizeof|sizeof
argument_list|(
name|oneline
argument_list|)
argument_list|,
name|stdin
argument_list|)
operator|&&
name|oneline
index|[
literal|0
index|]
operator|!=
literal|'\n'
condition|)
empty_stmt|;
comment|/* discard headers */
while|while
condition|(
name|fgets
argument_list|(
name|oneline
argument_list|,
sizeof|sizeof
argument_list|(
name|oneline
argument_list|)
argument_list|,
name|stdin
argument_list|)
operator|&&
name|oneline
index|[
literal|0
index|]
operator|==
literal|'\n'
condition|)
empty_stmt|;
comment|/* discard blanks */
name|insert_one_record
argument_list|(
name|log
argument_list|,
name|author
operator|+
literal|8
argument_list|,
name|oneline
argument_list|)
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|shortlog_add_commit
name|void
name|shortlog_add_commit
parameter_list|(
name|struct
name|shortlog
modifier|*
name|log
parameter_list|,
name|struct
name|commit
modifier|*
name|commit
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|author
init|=
name|NULL
decl_stmt|,
modifier|*
name|buffer
decl_stmt|;
name|buffer
operator|=
name|commit
operator|->
name|buffer
expr_stmt|;
while|while
condition|(
operator|*
name|buffer
operator|&&
operator|*
name|buffer
operator|!=
literal|'\n'
condition|)
block|{
specifier|const
name|char
modifier|*
name|eol
init|=
name|strchr
argument_list|(
name|buffer
argument_list|,
literal|'\n'
argument_list|)
decl_stmt|;
if|if
condition|(
name|eol
operator|==
name|NULL
condition|)
name|eol
operator|=
name|buffer
operator|+
name|strlen
argument_list|(
name|buffer
argument_list|)
expr_stmt|;
else|else
name|eol
operator|++
expr_stmt|;
if|if
condition|(
operator|!
name|prefixcmp
argument_list|(
name|buffer
argument_list|,
literal|"author "
argument_list|)
condition|)
name|author
operator|=
name|buffer
operator|+
literal|7
expr_stmt|;
name|buffer
operator|=
name|eol
expr_stmt|;
block|}
if|if
condition|(
operator|!
name|author
condition|)
name|die
argument_list|(
literal|"Missing author: %s"
argument_list|,
name|sha1_to_hex
argument_list|(
name|commit
operator|->
name|object
operator|.
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
name|log
operator|->
name|user_format
condition|)
block|{
name|struct
name|strbuf
name|buf
init|=
name|STRBUF_INIT
decl_stmt|;
name|pretty_print_commit
argument_list|(
name|CMIT_FMT_USERFORMAT
argument_list|,
name|commit
argument_list|,
operator|&
name|buf
argument_list|,
name|DEFAULT_ABBREV
argument_list|,
literal|""
argument_list|,
literal|""
argument_list|,
name|DATE_NORMAL
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|insert_one_record
argument_list|(
name|log
argument_list|,
name|author
argument_list|,
name|buf
operator|.
name|buf
argument_list|)
expr_stmt|;
name|strbuf_release
argument_list|(
operator|&
name|buf
argument_list|)
expr_stmt|;
return|return;
block|}
if|if
condition|(
operator|*
name|buffer
condition|)
name|buffer
operator|++
expr_stmt|;
name|insert_one_record
argument_list|(
name|log
argument_list|,
name|author
argument_list|,
operator|!
operator|*
name|buffer
condition|?
literal|"<none>"
else|:
name|buffer
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|get_from_rev
specifier|static
name|void
name|get_from_rev
parameter_list|(
name|struct
name|rev_info
modifier|*
name|rev
parameter_list|,
name|struct
name|shortlog
modifier|*
name|log
parameter_list|)
block|{
name|struct
name|commit
modifier|*
name|commit
decl_stmt|;
if|if
condition|(
name|prepare_revision_walk
argument_list|(
name|rev
argument_list|)
condition|)
name|die
argument_list|(
literal|"revision walk setup failed"
argument_list|)
expr_stmt|;
while|while
condition|(
operator|(
name|commit
operator|=
name|get_revision
argument_list|(
name|rev
argument_list|)
operator|)
operator|!=
name|NULL
condition|)
name|shortlog_add_commit
argument_list|(
name|log
argument_list|,
name|commit
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|parse_uint
specifier|static
name|int
name|parse_uint
parameter_list|(
name|char
specifier|const
modifier|*
modifier|*
name|arg
parameter_list|,
name|int
name|comma
parameter_list|,
name|int
name|defval
parameter_list|)
block|{
name|unsigned
name|long
name|ul
decl_stmt|;
name|int
name|ret
decl_stmt|;
name|char
modifier|*
name|endp
decl_stmt|;
name|ul
operator|=
name|strtoul
argument_list|(
operator|*
name|arg
argument_list|,
operator|&
name|endp
argument_list|,
literal|10
argument_list|)
expr_stmt|;
if|if
condition|(
operator|*
name|endp
operator|&&
operator|*
name|endp
operator|!=
name|comma
condition|)
return|return
operator|-
literal|1
return|;
if|if
condition|(
name|ul
operator|>
name|INT_MAX
condition|)
return|return
operator|-
literal|1
return|;
name|ret
operator|=
operator|*
name|arg
operator|==
name|endp
condition|?
name|defval
else|:
operator|(
name|int
operator|)
name|ul
expr_stmt|;
operator|*
name|arg
operator|=
operator|*
name|endp
condition|?
name|endp
operator|+
literal|1
else|:
name|endp
expr_stmt|;
return|return
name|ret
return|;
block|}
end_function
begin_decl_stmt
DECL|variable|wrap_arg_usage
specifier|static
specifier|const
name|char
name|wrap_arg_usage
index|[]
init|=
literal|"-w[<width>[,<indent1>[,<indent2>]]]"
decl_stmt|;
end_decl_stmt
begin_define
DECL|macro|DEFAULT_WRAPLEN
define|#
directive|define
name|DEFAULT_WRAPLEN
value|76
end_define
begin_define
DECL|macro|DEFAULT_INDENT1
define|#
directive|define
name|DEFAULT_INDENT1
value|6
end_define
begin_define
DECL|macro|DEFAULT_INDENT2
define|#
directive|define
name|DEFAULT_INDENT2
value|9
end_define
begin_function
DECL|function|parse_wrap_args
specifier|static
name|int
name|parse_wrap_args
parameter_list|(
specifier|const
name|struct
name|option
modifier|*
name|opt
parameter_list|,
specifier|const
name|char
modifier|*
name|arg
parameter_list|,
name|int
name|unset
parameter_list|)
block|{
name|struct
name|shortlog
modifier|*
name|log
init|=
name|opt
operator|->
name|value
decl_stmt|;
name|log
operator|->
name|wrap_lines
operator|=
operator|!
name|unset
expr_stmt|;
if|if
condition|(
name|unset
condition|)
return|return
literal|0
return|;
if|if
condition|(
operator|!
name|arg
condition|)
block|{
name|log
operator|->
name|wrap
operator|=
name|DEFAULT_WRAPLEN
expr_stmt|;
name|log
operator|->
name|in1
operator|=
name|DEFAULT_INDENT1
expr_stmt|;
name|log
operator|->
name|in2
operator|=
name|DEFAULT_INDENT2
expr_stmt|;
return|return
literal|0
return|;
block|}
name|log
operator|->
name|wrap
operator|=
name|parse_uint
argument_list|(
operator|&
name|arg
argument_list|,
literal|','
argument_list|,
name|DEFAULT_WRAPLEN
argument_list|)
expr_stmt|;
name|log
operator|->
name|in1
operator|=
name|parse_uint
argument_list|(
operator|&
name|arg
argument_list|,
literal|','
argument_list|,
name|DEFAULT_INDENT1
argument_list|)
expr_stmt|;
name|log
operator|->
name|in2
operator|=
name|parse_uint
argument_list|(
operator|&
name|arg
argument_list|,
literal|'\0'
argument_list|,
name|DEFAULT_INDENT2
argument_list|)
expr_stmt|;
if|if
condition|(
name|log
operator|->
name|wrap
operator|<
literal|0
operator|||
name|log
operator|->
name|in1
operator|<
literal|0
operator|||
name|log
operator|->
name|in2
operator|<
literal|0
condition|)
return|return
name|error
argument_list|(
name|wrap_arg_usage
argument_list|)
return|;
if|if
condition|(
name|log
operator|->
name|wrap
operator|&&
operator|(
operator|(
name|log
operator|->
name|in1
operator|&&
name|log
operator|->
name|wrap
operator|<=
name|log
operator|->
name|in1
operator|)
operator|||
operator|(
name|log
operator|->
name|in2
operator|&&
name|log
operator|->
name|wrap
operator|<=
name|log
operator|->
name|in2
operator|)
operator|)
condition|)
return|return
name|error
argument_list|(
name|wrap_arg_usage
argument_list|)
return|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|shortlog_init
name|void
name|shortlog_init
parameter_list|(
name|struct
name|shortlog
modifier|*
name|log
parameter_list|)
block|{
name|memset
argument_list|(
name|log
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
operator|*
name|log
argument_list|)
argument_list|)
expr_stmt|;
name|read_mailmap
argument_list|(
operator|&
name|log
operator|->
name|mailmap
argument_list|,
literal|".mailmap"
argument_list|,
operator|&
name|log
operator|->
name|common_repo_prefix
argument_list|)
expr_stmt|;
name|log
operator|->
name|list
operator|.
name|strdup_strings
operator|=
literal|1
expr_stmt|;
name|log
operator|->
name|wrap
operator|=
name|DEFAULT_WRAPLEN
expr_stmt|;
name|log
operator|->
name|in1
operator|=
name|DEFAULT_INDENT1
expr_stmt|;
name|log
operator|->
name|in2
operator|=
name|DEFAULT_INDENT2
expr_stmt|;
block|}
end_function
begin_function
DECL|function|cmd_shortlog
name|int
name|cmd_shortlog
parameter_list|(
name|int
name|argc
parameter_list|,
specifier|const
name|char
modifier|*
modifier|*
name|argv
parameter_list|,
specifier|const
name|char
modifier|*
name|prefix
parameter_list|)
block|{
specifier|static
name|struct
name|shortlog
name|log
decl_stmt|;
specifier|static
name|struct
name|rev_info
name|rev
decl_stmt|;
name|int
name|nongit
decl_stmt|;
specifier|static
specifier|const
name|struct
name|option
name|options
index|[]
init|=
block|{
name|OPT_BOOLEAN
argument_list|(
literal|'n'
argument_list|,
literal|"numbered"
argument_list|,
operator|&
name|log
operator|.
name|sort_by_number
argument_list|,
literal|"sort output according to the number of commits per author"
argument_list|)
block|,
name|OPT_BOOLEAN
argument_list|(
literal|'s'
argument_list|,
literal|"summary"
argument_list|,
operator|&
name|log
operator|.
name|summary
argument_list|,
literal|"Suppress commit descriptions, only provides commit count"
argument_list|)
block|,
name|OPT_BOOLEAN
argument_list|(
literal|'e'
argument_list|,
literal|"email"
argument_list|,
operator|&
name|log
operator|.
name|email
argument_list|,
literal|"Show the email address of each author"
argument_list|)
block|,
block|{
name|OPTION_CALLBACK
block|,
literal|'w'
block|,
name|NULL
block|,
operator|&
name|log
block|,
literal|"w[,i1[,i2]]"
block|,
literal|"Linewrap output"
block|,
name|PARSE_OPT_OPTARG
block|,
operator|&
name|parse_wrap_args
block|}
block|,
name|OPT_END
argument_list|()
block|, 	}
decl_stmt|;
name|struct
name|parse_opt_ctx_t
name|ctx
decl_stmt|;
name|prefix
operator|=
name|setup_git_directory_gently
argument_list|(
operator|&
name|nongit
argument_list|)
expr_stmt|;
name|shortlog_init
argument_list|(
operator|&
name|log
argument_list|)
expr_stmt|;
name|init_revisions
argument_list|(
operator|&
name|rev
argument_list|,
name|prefix
argument_list|)
expr_stmt|;
name|parse_options_start
argument_list|(
operator|&
name|ctx
argument_list|,
name|argc
argument_list|,
name|argv
argument_list|,
name|PARSE_OPT_KEEP_DASHDASH
operator||
name|PARSE_OPT_KEEP_ARGV0
argument_list|)
expr_stmt|;
for|for
control|(
init|;
condition|;
control|)
block|{
switch|switch
condition|(
name|parse_options_step
argument_list|(
operator|&
name|ctx
argument_list|,
name|options
argument_list|,
name|shortlog_usage
argument_list|)
condition|)
block|{
case|case
name|PARSE_OPT_HELP
case|:
name|exit
argument_list|(
literal|129
argument_list|)
expr_stmt|;
case|case
name|PARSE_OPT_DONE
case|:
goto|goto
name|parse_done
goto|;
block|}
name|parse_revision_opt
argument_list|(
operator|&
name|rev
argument_list|,
operator|&
name|ctx
argument_list|,
name|options
argument_list|,
name|shortlog_usage
argument_list|)
expr_stmt|;
block|}
name|parse_done
label|:
name|argc
operator|=
name|parse_options_end
argument_list|(
operator|&
name|ctx
argument_list|)
expr_stmt|;
if|if
condition|(
name|setup_revisions
argument_list|(
name|argc
argument_list|,
name|argv
argument_list|,
operator|&
name|rev
argument_list|,
name|NULL
argument_list|)
operator|!=
literal|1
condition|)
block|{
name|error
argument_list|(
literal|"unrecognized argument: %s"
argument_list|,
name|argv
index|[
literal|1
index|]
argument_list|)
expr_stmt|;
name|usage_with_options
argument_list|(
name|shortlog_usage
argument_list|,
name|options
argument_list|)
expr_stmt|;
block|}
name|log
operator|.
name|user_format
operator|=
name|rev
operator|.
name|commit_format
operator|==
name|CMIT_FMT_USERFORMAT
expr_stmt|;
comment|/* assume HEAD if from a tty */
if|if
condition|(
operator|!
name|nongit
operator|&&
operator|!
name|rev
operator|.
name|pending
operator|.
name|nr
operator|&&
name|isatty
argument_list|(
literal|0
argument_list|)
condition|)
name|add_head_to_pending
argument_list|(
operator|&
name|rev
argument_list|)
expr_stmt|;
if|if
condition|(
name|rev
operator|.
name|pending
operator|.
name|nr
operator|==
literal|0
condition|)
block|{
name|read_from_stdin
argument_list|(
operator|&
name|log
argument_list|)
expr_stmt|;
block|}
else|else
name|get_from_rev
argument_list|(
operator|&
name|rev
argument_list|,
operator|&
name|log
argument_list|)
expr_stmt|;
name|shortlog_output
argument_list|(
operator|&
name|log
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|shortlog_output
name|void
name|shortlog_output
parameter_list|(
name|struct
name|shortlog
modifier|*
name|log
parameter_list|)
block|{
name|int
name|i
decl_stmt|,
name|j
decl_stmt|;
if|if
condition|(
name|log
operator|->
name|sort_by_number
condition|)
name|qsort
argument_list|(
name|log
operator|->
name|list
operator|.
name|items
argument_list|,
name|log
operator|->
name|list
operator|.
name|nr
argument_list|,
sizeof|sizeof
argument_list|(
expr|struct
name|string_list_item
argument_list|)
argument_list|,
name|compare_by_number
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
name|log
operator|->
name|list
operator|.
name|nr
condition|;
name|i
operator|++
control|)
block|{
name|struct
name|string_list
modifier|*
name|onelines
init|=
name|log
operator|->
name|list
operator|.
name|items
index|[
name|i
index|]
operator|.
name|util
decl_stmt|;
if|if
condition|(
name|log
operator|->
name|summary
condition|)
block|{
name|printf
argument_list|(
literal|"%6d\t%s\n"
argument_list|,
name|onelines
operator|->
name|nr
argument_list|,
name|log
operator|->
name|list
operator|.
name|items
index|[
name|i
index|]
operator|.
name|string
argument_list|)
expr_stmt|;
block|}
else|else
block|{
name|printf
argument_list|(
literal|"%s (%d):\n"
argument_list|,
name|log
operator|->
name|list
operator|.
name|items
index|[
name|i
index|]
operator|.
name|string
argument_list|,
name|onelines
operator|->
name|nr
argument_list|)
expr_stmt|;
for|for
control|(
name|j
operator|=
name|onelines
operator|->
name|nr
operator|-
literal|1
init|;
name|j
operator|>=
literal|0
condition|;
name|j
operator|--
control|)
block|{
specifier|const
name|char
modifier|*
name|msg
init|=
name|onelines
operator|->
name|items
index|[
name|j
index|]
operator|.
name|string
decl_stmt|;
if|if
condition|(
name|log
operator|->
name|wrap_lines
condition|)
block|{
name|int
name|col
init|=
name|print_wrapped_text
argument_list|(
name|msg
argument_list|,
name|log
operator|->
name|in1
argument_list|,
name|log
operator|->
name|in2
argument_list|,
name|log
operator|->
name|wrap
argument_list|)
decl_stmt|;
if|if
condition|(
name|col
operator|!=
name|log
operator|->
name|wrap
condition|)
name|putchar
argument_list|(
literal|'\n'
argument_list|)
expr_stmt|;
block|}
else|else
name|printf
argument_list|(
literal|"      %s\n"
argument_list|,
name|msg
argument_list|)
expr_stmt|;
block|}
name|putchar
argument_list|(
literal|'\n'
argument_list|)
expr_stmt|;
block|}
name|onelines
operator|->
name|strdup_strings
operator|=
literal|1
expr_stmt|;
name|string_list_clear
argument_list|(
name|onelines
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|onelines
argument_list|)
expr_stmt|;
name|log
operator|->
name|list
operator|.
name|items
index|[
name|i
index|]
operator|.
name|util
operator|=
name|NULL
expr_stmt|;
block|}
name|log
operator|->
name|list
operator|.
name|strdup_strings
operator|=
literal|1
expr_stmt|;
name|string_list_clear
argument_list|(
operator|&
name|log
operator|->
name|list
argument_list|,
literal|1
argument_list|)
expr_stmt|;
name|log
operator|->
name|mailmap
operator|.
name|strdup_strings
operator|=
literal|1
expr_stmt|;
name|string_list_clear
argument_list|(
operator|&
name|log
operator|->
name|mailmap
argument_list|,
literal|1
argument_list|)
expr_stmt|;
block|}
end_function
end_unit
