begin_unit
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
file|"epoch.h"
end_include
begin_define
DECL|macro|SEEN
define|#
directive|define
name|SEEN
value|(1u<< 0)
end_define
begin_define
DECL|macro|INTERESTING
define|#
directive|define
name|INTERESTING
value|(1u<< 1)
end_define
begin_define
DECL|macro|COUNTED
define|#
directive|define
name|COUNTED
value|(1u<< 2)
end_define
begin_decl_stmt
DECL|variable|rev_list_usage
specifier|static
specifier|const
name|char
name|rev_list_usage
index|[]
init|=
literal|"usage: git-rev-list [OPTION] commit-id<commit-id>\n"
literal|"  --max-count=nr\n"
literal|"  --max-age=epoch\n"
literal|"  --min-age=epoch\n"
literal|"  --header\n"
literal|"  --pretty\n"
literal|"  --merge-order [ --show-breaks ]"
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|bisect_list
specifier|static
name|int
name|bisect_list
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|verbose_header
specifier|static
name|int
name|verbose_header
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|show_parents
specifier|static
name|int
name|show_parents
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|hdr_termination
specifier|static
name|int
name|hdr_termination
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|prefix
specifier|static
specifier|const
name|char
modifier|*
name|prefix
init|=
literal|""
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|max_age
specifier|static
name|unsigned
name|long
name|max_age
init|=
operator|-
literal|1
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|min_age
specifier|static
name|unsigned
name|long
name|min_age
init|=
operator|-
literal|1
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|max_count
specifier|static
name|int
name|max_count
init|=
operator|-
literal|1
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|commit_format
specifier|static
name|enum
name|cmit_fmt
name|commit_format
init|=
name|CMIT_FMT_RAW
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|merge_order
specifier|static
name|int
name|merge_order
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|show_breaks
specifier|static
name|int
name|show_breaks
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|show_commit
specifier|static
name|void
name|show_commit
parameter_list|(
name|struct
name|commit
modifier|*
name|commit
parameter_list|)
block|{
if|if
condition|(
name|show_breaks
condition|)
block|{
name|prefix
operator|=
literal|"| "
expr_stmt|;
if|if
condition|(
name|commit
operator|->
name|object
operator|.
name|flags
operator|&
name|DISCONTINUITY
condition|)
block|{
name|prefix
operator|=
literal|"^ "
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|commit
operator|->
name|object
operator|.
name|flags
operator|&
name|BOUNDARY
condition|)
block|{
name|prefix
operator|=
literal|"= "
expr_stmt|;
block|}
block|}
name|printf
argument_list|(
literal|"%s%s"
argument_list|,
name|prefix
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
name|show_parents
condition|)
block|{
name|struct
name|commit_list
modifier|*
name|parents
init|=
name|commit
operator|->
name|parents
decl_stmt|;
while|while
condition|(
name|parents
condition|)
block|{
name|printf
argument_list|(
literal|" %s"
argument_list|,
name|sha1_to_hex
argument_list|(
name|parents
operator|->
name|item
operator|->
name|object
operator|.
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
name|parents
operator|=
name|parents
operator|->
name|next
expr_stmt|;
block|}
block|}
name|putchar
argument_list|(
literal|'\n'
argument_list|)
expr_stmt|;
if|if
condition|(
name|verbose_header
condition|)
block|{
specifier|static
name|char
name|pretty_header
index|[
literal|16384
index|]
decl_stmt|;
name|pretty_print_commit
argument_list|(
name|commit_format
argument_list|,
name|commit
operator|->
name|buffer
argument_list|,
operator|~
literal|0
argument_list|,
name|pretty_header
argument_list|,
sizeof|sizeof
argument_list|(
name|pretty_header
argument_list|)
argument_list|)
expr_stmt|;
name|printf
argument_list|(
literal|"%s%c"
argument_list|,
name|pretty_header
argument_list|,
name|hdr_termination
argument_list|)
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|filter_commit
specifier|static
name|int
name|filter_commit
parameter_list|(
name|struct
name|commit
modifier|*
name|commit
parameter_list|)
block|{
if|if
condition|(
name|commit
operator|->
name|object
operator|.
name|flags
operator|&
name|UNINTERESTING
condition|)
return|return
name|CONTINUE
return|;
if|if
condition|(
name|min_age
operator|!=
operator|-
literal|1
operator|&&
operator|(
name|commit
operator|->
name|date
operator|>
name|min_age
operator|)
condition|)
return|return
name|CONTINUE
return|;
if|if
condition|(
name|max_age
operator|!=
operator|-
literal|1
operator|&&
operator|(
name|commit
operator|->
name|date
operator|<
name|max_age
operator|)
condition|)
return|return
name|STOP
return|;
if|if
condition|(
name|max_count
operator|!=
operator|-
literal|1
operator|&&
operator|!
name|max_count
operator|--
condition|)
return|return
name|STOP
return|;
return|return
name|DO
return|;
block|}
end_function
begin_function
DECL|function|process_commit
specifier|static
name|int
name|process_commit
parameter_list|(
name|struct
name|commit
modifier|*
name|commit
parameter_list|)
block|{
name|int
name|action
init|=
name|filter_commit
argument_list|(
name|commit
argument_list|)
decl_stmt|;
if|if
condition|(
name|action
operator|==
name|STOP
condition|)
block|{
return|return
name|STOP
return|;
block|}
if|if
condition|(
name|action
operator|==
name|CONTINUE
condition|)
block|{
return|return
name|CONTINUE
return|;
block|}
name|show_commit
argument_list|(
name|commit
argument_list|)
expr_stmt|;
return|return
name|CONTINUE
return|;
block|}
end_function
begin_function
DECL|function|show_commit_list
specifier|static
name|void
name|show_commit_list
parameter_list|(
name|struct
name|commit_list
modifier|*
name|list
parameter_list|)
block|{
while|while
condition|(
name|list
condition|)
block|{
name|struct
name|commit
modifier|*
name|commit
init|=
name|pop_most_recent_commit
argument_list|(
operator|&
name|list
argument_list|,
name|SEEN
argument_list|)
decl_stmt|;
if|if
condition|(
name|process_commit
argument_list|(
name|commit
argument_list|)
operator|==
name|STOP
condition|)
break|break;
block|}
block|}
end_function
begin_function
DECL|function|mark_parents_uninteresting
specifier|static
name|void
name|mark_parents_uninteresting
parameter_list|(
name|struct
name|commit
modifier|*
name|commit
parameter_list|)
block|{
name|struct
name|commit_list
modifier|*
name|parents
init|=
name|commit
operator|->
name|parents
decl_stmt|;
while|while
condition|(
name|parents
condition|)
block|{
name|struct
name|commit
modifier|*
name|commit
init|=
name|parents
operator|->
name|item
decl_stmt|;
name|commit
operator|->
name|object
operator|.
name|flags
operator||=
name|UNINTERESTING
expr_stmt|;
name|parents
operator|=
name|parents
operator|->
name|next
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|everybody_uninteresting
specifier|static
name|int
name|everybody_uninteresting
parameter_list|(
name|struct
name|commit_list
modifier|*
name|list
parameter_list|)
block|{
while|while
condition|(
name|list
condition|)
block|{
name|struct
name|commit
modifier|*
name|commit
init|=
name|list
operator|->
name|item
decl_stmt|;
name|list
operator|=
name|list
operator|->
name|next
expr_stmt|;
if|if
condition|(
name|commit
operator|->
name|object
operator|.
name|flags
operator|&
name|UNINTERESTING
condition|)
continue|continue;
return|return
literal|0
return|;
block|}
return|return
literal|1
return|;
block|}
end_function
begin_comment
comment|/*  * This is a truly stupid algorithm, but it's only  * used for bisection, and we just don't care enough.  *  * We care just barely enough to avoid recursing for  * non-merge entries.  */
end_comment
begin_function
DECL|function|count_distance
specifier|static
name|int
name|count_distance
parameter_list|(
name|struct
name|commit_list
modifier|*
name|entry
parameter_list|)
block|{
name|int
name|nr
init|=
literal|0
decl_stmt|;
while|while
condition|(
name|entry
condition|)
block|{
name|struct
name|commit
modifier|*
name|commit
init|=
name|entry
operator|->
name|item
decl_stmt|;
name|struct
name|commit_list
modifier|*
name|p
decl_stmt|;
if|if
condition|(
name|commit
operator|->
name|object
operator|.
name|flags
operator|&
operator|(
name|UNINTERESTING
operator||
name|COUNTED
operator|)
condition|)
break|break;
name|nr
operator|++
expr_stmt|;
name|commit
operator|->
name|object
operator|.
name|flags
operator||=
name|COUNTED
expr_stmt|;
name|p
operator|=
name|commit
operator|->
name|parents
expr_stmt|;
name|entry
operator|=
name|p
expr_stmt|;
if|if
condition|(
name|p
condition|)
block|{
name|p
operator|=
name|p
operator|->
name|next
expr_stmt|;
while|while
condition|(
name|p
condition|)
block|{
name|nr
operator|+=
name|count_distance
argument_list|(
name|p
argument_list|)
expr_stmt|;
name|p
operator|=
name|p
operator|->
name|next
expr_stmt|;
block|}
block|}
block|}
return|return
name|nr
return|;
block|}
end_function
begin_function
DECL|function|clear_distance
specifier|static
name|void
name|clear_distance
parameter_list|(
name|struct
name|commit_list
modifier|*
name|list
parameter_list|)
block|{
while|while
condition|(
name|list
condition|)
block|{
name|struct
name|commit
modifier|*
name|commit
init|=
name|list
operator|->
name|item
decl_stmt|;
name|commit
operator|->
name|object
operator|.
name|flags
operator|&=
operator|~
name|COUNTED
expr_stmt|;
name|list
operator|=
name|list
operator|->
name|next
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|find_bisection
specifier|static
name|struct
name|commit_list
modifier|*
name|find_bisection
parameter_list|(
name|struct
name|commit_list
modifier|*
name|list
parameter_list|)
block|{
name|int
name|nr
decl_stmt|,
name|closest
decl_stmt|;
name|struct
name|commit_list
modifier|*
name|p
decl_stmt|,
modifier|*
name|best
decl_stmt|;
name|nr
operator|=
literal|0
expr_stmt|;
name|p
operator|=
name|list
expr_stmt|;
while|while
condition|(
name|p
condition|)
block|{
name|nr
operator|++
expr_stmt|;
name|p
operator|=
name|p
operator|->
name|next
expr_stmt|;
block|}
name|closest
operator|=
literal|0
expr_stmt|;
name|best
operator|=
name|list
expr_stmt|;
name|p
operator|=
name|list
expr_stmt|;
while|while
condition|(
name|p
condition|)
block|{
name|int
name|distance
init|=
name|count_distance
argument_list|(
name|p
argument_list|)
decl_stmt|;
name|clear_distance
argument_list|(
name|list
argument_list|)
expr_stmt|;
if|if
condition|(
name|nr
operator|-
name|distance
operator|<
name|distance
condition|)
name|distance
operator|=
name|nr
operator|-
name|distance
expr_stmt|;
if|if
condition|(
name|distance
operator|>
name|closest
condition|)
block|{
name|best
operator|=
name|p
expr_stmt|;
name|closest
operator|=
name|distance
expr_stmt|;
block|}
name|p
operator|=
name|p
operator|->
name|next
expr_stmt|;
block|}
if|if
condition|(
name|best
condition|)
name|best
operator|->
name|next
operator|=
name|NULL
expr_stmt|;
return|return
name|best
return|;
block|}
end_function
begin_function
DECL|function|limit_list
name|struct
name|commit_list
modifier|*
name|limit_list
parameter_list|(
name|struct
name|commit_list
modifier|*
name|list
parameter_list|)
block|{
name|struct
name|commit_list
modifier|*
name|newlist
init|=
name|NULL
decl_stmt|;
name|struct
name|commit_list
modifier|*
modifier|*
name|p
init|=
operator|&
name|newlist
decl_stmt|;
do|do
block|{
name|struct
name|commit
modifier|*
name|commit
init|=
name|pop_most_recent_commit
argument_list|(
operator|&
name|list
argument_list|,
name|SEEN
argument_list|)
decl_stmt|;
name|struct
name|object
modifier|*
name|obj
init|=
operator|&
name|commit
operator|->
name|object
decl_stmt|;
if|if
condition|(
name|obj
operator|->
name|flags
operator|&
name|UNINTERESTING
condition|)
block|{
name|mark_parents_uninteresting
argument_list|(
name|commit
argument_list|)
expr_stmt|;
if|if
condition|(
name|everybody_uninteresting
argument_list|(
name|list
argument_list|)
condition|)
break|break;
continue|continue;
block|}
name|p
operator|=
operator|&
name|commit_list_insert
argument_list|(
name|commit
argument_list|,
name|p
argument_list|)
operator|->
name|next
expr_stmt|;
block|}
do|while
condition|(
name|list
condition|)
do|;
if|if
condition|(
name|bisect_list
condition|)
name|newlist
operator|=
name|find_bisection
argument_list|(
name|newlist
argument_list|)
expr_stmt|;
return|return
name|newlist
return|;
block|}
end_function
begin_function
DECL|function|get_commit_format
specifier|static
name|enum
name|cmit_fmt
name|get_commit_format
parameter_list|(
specifier|const
name|char
modifier|*
name|arg
parameter_list|)
block|{
if|if
condition|(
operator|!
operator|*
name|arg
condition|)
return|return
name|CMIT_FMT_DEFAULT
return|;
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"=raw"
argument_list|)
condition|)
return|return
name|CMIT_FMT_RAW
return|;
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"=medium"
argument_list|)
condition|)
return|return
name|CMIT_FMT_MEDIUM
return|;
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"=short"
argument_list|)
condition|)
return|return
name|CMIT_FMT_SHORT
return|;
name|usage
argument_list|(
name|rev_list_usage
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|main
name|int
name|main
parameter_list|(
name|int
name|argc
parameter_list|,
name|char
modifier|*
modifier|*
name|argv
parameter_list|)
block|{
name|struct
name|commit_list
modifier|*
name|list
init|=
name|NULL
decl_stmt|;
name|int
name|i
decl_stmt|,
name|limited
init|=
literal|0
decl_stmt|;
for|for
control|(
name|i
operator|=
literal|1
init|;
name|i
operator|<
name|argc
condition|;
name|i
operator|++
control|)
block|{
name|int
name|flags
decl_stmt|;
name|char
modifier|*
name|arg
init|=
name|argv
index|[
name|i
index|]
decl_stmt|;
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
name|struct
name|commit
modifier|*
name|commit
decl_stmt|;
if|if
condition|(
operator|!
name|strncmp
argument_list|(
name|arg
argument_list|,
literal|"--max-count="
argument_list|,
literal|12
argument_list|)
condition|)
block|{
name|max_count
operator|=
name|atoi
argument_list|(
name|arg
operator|+
literal|12
argument_list|)
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|strncmp
argument_list|(
name|arg
argument_list|,
literal|"--max-age="
argument_list|,
literal|10
argument_list|)
condition|)
block|{
name|max_age
operator|=
name|atoi
argument_list|(
name|arg
operator|+
literal|10
argument_list|)
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|strncmp
argument_list|(
name|arg
argument_list|,
literal|"--min-age="
argument_list|,
literal|10
argument_list|)
condition|)
block|{
name|min_age
operator|=
name|atoi
argument_list|(
name|arg
operator|+
literal|10
argument_list|)
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"--header"
argument_list|)
condition|)
block|{
name|verbose_header
operator|=
literal|1
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|strncmp
argument_list|(
name|arg
argument_list|,
literal|"--pretty"
argument_list|,
literal|8
argument_list|)
condition|)
block|{
name|commit_format
operator|=
name|get_commit_format
argument_list|(
name|arg
operator|+
literal|8
argument_list|)
expr_stmt|;
name|verbose_header
operator|=
literal|1
expr_stmt|;
name|hdr_termination
operator|=
literal|'\n'
expr_stmt|;
name|prefix
operator|=
literal|"commit "
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"--parents"
argument_list|)
condition|)
block|{
name|show_parents
operator|=
literal|1
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"--bisect"
argument_list|)
condition|)
block|{
name|bisect_list
operator|=
literal|1
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|strncmp
argument_list|(
name|arg
argument_list|,
literal|"--merge-order"
argument_list|,
literal|13
argument_list|)
condition|)
block|{
name|merge_order
operator|=
literal|1
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|strncmp
argument_list|(
name|arg
argument_list|,
literal|"--show-breaks"
argument_list|,
literal|13
argument_list|)
condition|)
block|{
name|show_breaks
operator|=
literal|1
expr_stmt|;
continue|continue;
block|}
name|flags
operator|=
literal|0
expr_stmt|;
if|if
condition|(
operator|*
name|arg
operator|==
literal|'^'
condition|)
block|{
name|flags
operator|=
name|UNINTERESTING
expr_stmt|;
name|arg
operator|++
expr_stmt|;
name|limited
operator|=
literal|1
expr_stmt|;
block|}
if|if
condition|(
name|get_sha1
argument_list|(
name|arg
argument_list|,
name|sha1
argument_list|)
operator|||
operator|(
name|show_breaks
operator|&&
operator|!
name|merge_order
operator|)
condition|)
name|usage
argument_list|(
name|rev_list_usage
argument_list|)
expr_stmt|;
name|commit
operator|=
name|lookup_commit_reference
argument_list|(
name|sha1
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|commit
operator|||
name|parse_commit
argument_list|(
name|commit
argument_list|)
operator|<
literal|0
condition|)
name|die
argument_list|(
literal|"bad commit object %s"
argument_list|,
name|arg
argument_list|)
expr_stmt|;
name|commit
operator|->
name|object
operator|.
name|flags
operator||=
name|flags
expr_stmt|;
name|commit_list_insert
argument_list|(
name|commit
argument_list|,
operator|&
name|list
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
operator|!
name|list
condition|)
name|usage
argument_list|(
name|rev_list_usage
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|merge_order
condition|)
block|{
if|if
condition|(
name|limited
condition|)
name|list
operator|=
name|limit_list
argument_list|(
name|list
argument_list|)
expr_stmt|;
name|show_commit_list
argument_list|(
name|list
argument_list|)
expr_stmt|;
block|}
else|else
block|{
if|if
condition|(
name|sort_list_in_merge_order
argument_list|(
name|list
argument_list|,
operator|&
name|process_commit
argument_list|)
condition|)
block|{
name|die
argument_list|(
literal|"merge order sort failed\n"
argument_list|)
expr_stmt|;
block|}
block|}
return|return
literal|0
return|;
block|}
end_function
end_unit
