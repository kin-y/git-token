begin_unit
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"wt-status.h"
end_include
begin_include
include|#
directive|include
file|"color.h"
end_include
begin_include
include|#
directive|include
file|"object.h"
end_include
begin_include
include|#
directive|include
file|"dir.h"
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
file|"revision.h"
end_include
begin_include
include|#
directive|include
file|"diffcore.h"
end_include
begin_include
include|#
directive|include
file|"quote.h"
end_include
begin_include
include|#
directive|include
file|"run-command.h"
end_include
begin_include
include|#
directive|include
file|"remote.h"
end_include
begin_decl_stmt
DECL|variable|wt_status_relative_paths
name|int
name|wt_status_relative_paths
init|=
literal|1
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|wt_status_use_color
name|int
name|wt_status_use_color
init|=
operator|-
literal|1
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|wt_status_submodule_summary
name|int
name|wt_status_submodule_summary
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|wt_status_colors
specifier|static
name|char
name|wt_status_colors
index|[]
index|[
name|COLOR_MAXLEN
index|]
init|=
block|{
literal|""
block|,
comment|/* WT_STATUS_HEADER: normal */
literal|"\033[32m"
block|,
comment|/* WT_STATUS_UPDATED: green */
literal|"\033[31m"
block|,
comment|/* WT_STATUS_CHANGED: red */
literal|"\033[31m"
block|,
comment|/* WT_STATUS_UNTRACKED: red */
literal|"\033[31m"
block|,
comment|/* WT_STATUS_NOBRANCH: red */
block|}
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|show_untracked_files
name|enum
name|untracked_status_type
name|show_untracked_files
init|=
name|SHOW_NORMAL_UNTRACKED_FILES
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|parse_status_slot
specifier|static
name|int
name|parse_status_slot
parameter_list|(
specifier|const
name|char
modifier|*
name|var
parameter_list|,
name|int
name|offset
parameter_list|)
block|{
if|if
condition|(
operator|!
name|strcasecmp
argument_list|(
name|var
operator|+
name|offset
argument_list|,
literal|"header"
argument_list|)
condition|)
return|return
name|WT_STATUS_HEADER
return|;
if|if
condition|(
operator|!
name|strcasecmp
argument_list|(
name|var
operator|+
name|offset
argument_list|,
literal|"updated"
argument_list|)
operator|||
operator|!
name|strcasecmp
argument_list|(
name|var
operator|+
name|offset
argument_list|,
literal|"added"
argument_list|)
condition|)
return|return
name|WT_STATUS_UPDATED
return|;
if|if
condition|(
operator|!
name|strcasecmp
argument_list|(
name|var
operator|+
name|offset
argument_list|,
literal|"changed"
argument_list|)
condition|)
return|return
name|WT_STATUS_CHANGED
return|;
if|if
condition|(
operator|!
name|strcasecmp
argument_list|(
name|var
operator|+
name|offset
argument_list|,
literal|"untracked"
argument_list|)
condition|)
return|return
name|WT_STATUS_UNTRACKED
return|;
if|if
condition|(
operator|!
name|strcasecmp
argument_list|(
name|var
operator|+
name|offset
argument_list|,
literal|"nobranch"
argument_list|)
condition|)
return|return
name|WT_STATUS_NOBRANCH
return|;
name|die
argument_list|(
literal|"bad config variable '%s'"
argument_list|,
name|var
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|color
specifier|static
specifier|const
name|char
modifier|*
name|color
parameter_list|(
name|int
name|slot
parameter_list|)
block|{
return|return
name|wt_status_use_color
operator|>
literal|0
condition|?
name|wt_status_colors
index|[
name|slot
index|]
else|:
literal|""
return|;
block|}
end_function
begin_function
DECL|function|wt_status_prepare
name|void
name|wt_status_prepare
parameter_list|(
name|struct
name|wt_status
modifier|*
name|s
parameter_list|)
block|{
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
specifier|const
name|char
modifier|*
name|head
decl_stmt|;
name|memset
argument_list|(
name|s
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
operator|*
name|s
argument_list|)
argument_list|)
expr_stmt|;
name|head
operator|=
name|resolve_ref
argument_list|(
literal|"HEAD"
argument_list|,
name|sha1
argument_list|,
literal|0
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|s
operator|->
name|branch
operator|=
name|head
condition|?
name|xstrdup
argument_list|(
name|head
argument_list|)
else|:
name|NULL
expr_stmt|;
name|s
operator|->
name|reference
operator|=
literal|"HEAD"
expr_stmt|;
name|s
operator|->
name|fp
operator|=
name|stdout
expr_stmt|;
name|s
operator|->
name|index_file
operator|=
name|get_index_file
argument_list|()
expr_stmt|;
block|}
end_function
begin_function
DECL|function|wt_status_print_cached_header
specifier|static
name|void
name|wt_status_print_cached_header
parameter_list|(
name|struct
name|wt_status
modifier|*
name|s
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|c
init|=
name|color
argument_list|(
name|WT_STATUS_HEADER
argument_list|)
decl_stmt|;
name|color_fprintf_ln
argument_list|(
name|s
operator|->
name|fp
argument_list|,
name|c
argument_list|,
literal|"# Changes to be committed:"
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|s
operator|->
name|is_initial
condition|)
block|{
name|color_fprintf_ln
argument_list|(
name|s
operator|->
name|fp
argument_list|,
name|c
argument_list|,
literal|"#   (use \"git reset %s<file>...\" to unstage)"
argument_list|,
name|s
operator|->
name|reference
argument_list|)
expr_stmt|;
block|}
else|else
block|{
name|color_fprintf_ln
argument_list|(
name|s
operator|->
name|fp
argument_list|,
name|c
argument_list|,
literal|"#   (use \"git rm --cached<file>...\" to unstage)"
argument_list|)
expr_stmt|;
block|}
name|color_fprintf_ln
argument_list|(
name|s
operator|->
name|fp
argument_list|,
name|c
argument_list|,
literal|"#"
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|wt_status_print_dirty_header
specifier|static
name|void
name|wt_status_print_dirty_header
parameter_list|(
name|struct
name|wt_status
modifier|*
name|s
parameter_list|,
name|int
name|has_deleted
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|c
init|=
name|color
argument_list|(
name|WT_STATUS_HEADER
argument_list|)
decl_stmt|;
name|color_fprintf_ln
argument_list|(
name|s
operator|->
name|fp
argument_list|,
name|c
argument_list|,
literal|"# Changed but not updated:"
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|has_deleted
condition|)
name|color_fprintf_ln
argument_list|(
name|s
operator|->
name|fp
argument_list|,
name|c
argument_list|,
literal|"#   (use \"git add<file>...\" to update what will be committed)"
argument_list|)
expr_stmt|;
else|else
name|color_fprintf_ln
argument_list|(
name|s
operator|->
name|fp
argument_list|,
name|c
argument_list|,
literal|"#   (use \"git add/rm<file>...\" to update what will be committed)"
argument_list|)
expr_stmt|;
name|color_fprintf_ln
argument_list|(
name|s
operator|->
name|fp
argument_list|,
name|c
argument_list|,
literal|"#   (use \"git checkout --<file>...\" to discard changes in working directory)"
argument_list|)
expr_stmt|;
name|color_fprintf_ln
argument_list|(
name|s
operator|->
name|fp
argument_list|,
name|c
argument_list|,
literal|"#"
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|wt_status_print_untracked_header
specifier|static
name|void
name|wt_status_print_untracked_header
parameter_list|(
name|struct
name|wt_status
modifier|*
name|s
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|c
init|=
name|color
argument_list|(
name|WT_STATUS_HEADER
argument_list|)
decl_stmt|;
name|color_fprintf_ln
argument_list|(
name|s
operator|->
name|fp
argument_list|,
name|c
argument_list|,
literal|"# Untracked files:"
argument_list|)
expr_stmt|;
name|color_fprintf_ln
argument_list|(
name|s
operator|->
name|fp
argument_list|,
name|c
argument_list|,
literal|"#   (use \"git add<file>...\" to include in what will be committed)"
argument_list|)
expr_stmt|;
name|color_fprintf_ln
argument_list|(
name|s
operator|->
name|fp
argument_list|,
name|c
argument_list|,
literal|"#"
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|wt_status_print_trailer
specifier|static
name|void
name|wt_status_print_trailer
parameter_list|(
name|struct
name|wt_status
modifier|*
name|s
parameter_list|)
block|{
name|color_fprintf_ln
argument_list|(
name|s
operator|->
name|fp
argument_list|,
name|color
argument_list|(
name|WT_STATUS_HEADER
argument_list|)
argument_list|,
literal|"#"
argument_list|)
expr_stmt|;
block|}
end_function
begin_define
DECL|macro|quote_path
define|#
directive|define
name|quote_path
value|quote_path_relative
end_define
begin_function
DECL|function|wt_status_print_filepair
specifier|static
name|void
name|wt_status_print_filepair
parameter_list|(
name|struct
name|wt_status
modifier|*
name|s
parameter_list|,
name|int
name|t
parameter_list|,
name|struct
name|diff_filepair
modifier|*
name|p
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|c
init|=
name|color
argument_list|(
name|t
argument_list|)
decl_stmt|;
specifier|const
name|char
modifier|*
name|one
decl_stmt|,
modifier|*
name|two
decl_stmt|;
name|struct
name|strbuf
name|onebuf
init|=
name|STRBUF_INIT
decl_stmt|,
name|twobuf
init|=
name|STRBUF_INIT
decl_stmt|;
name|one
operator|=
name|quote_path
argument_list|(
name|p
operator|->
name|one
operator|->
name|path
argument_list|,
operator|-
literal|1
argument_list|,
operator|&
name|onebuf
argument_list|,
name|s
operator|->
name|prefix
argument_list|)
expr_stmt|;
name|two
operator|=
name|quote_path
argument_list|(
name|p
operator|->
name|two
operator|->
name|path
argument_list|,
operator|-
literal|1
argument_list|,
operator|&
name|twobuf
argument_list|,
name|s
operator|->
name|prefix
argument_list|)
expr_stmt|;
name|color_fprintf
argument_list|(
name|s
operator|->
name|fp
argument_list|,
name|color
argument_list|(
name|WT_STATUS_HEADER
argument_list|)
argument_list|,
literal|"#\t"
argument_list|)
expr_stmt|;
switch|switch
condition|(
name|p
operator|->
name|status
condition|)
block|{
case|case
name|DIFF_STATUS_ADDED
case|:
name|color_fprintf
argument_list|(
name|s
operator|->
name|fp
argument_list|,
name|c
argument_list|,
literal|"new file:   %s"
argument_list|,
name|one
argument_list|)
expr_stmt|;
break|break;
case|case
name|DIFF_STATUS_COPIED
case|:
name|color_fprintf
argument_list|(
name|s
operator|->
name|fp
argument_list|,
name|c
argument_list|,
literal|"copied:     %s -> %s"
argument_list|,
name|one
argument_list|,
name|two
argument_list|)
expr_stmt|;
break|break;
case|case
name|DIFF_STATUS_DELETED
case|:
name|color_fprintf
argument_list|(
name|s
operator|->
name|fp
argument_list|,
name|c
argument_list|,
literal|"deleted:    %s"
argument_list|,
name|one
argument_list|)
expr_stmt|;
break|break;
case|case
name|DIFF_STATUS_MODIFIED
case|:
name|color_fprintf
argument_list|(
name|s
operator|->
name|fp
argument_list|,
name|c
argument_list|,
literal|"modified:   %s"
argument_list|,
name|one
argument_list|)
expr_stmt|;
break|break;
case|case
name|DIFF_STATUS_RENAMED
case|:
name|color_fprintf
argument_list|(
name|s
operator|->
name|fp
argument_list|,
name|c
argument_list|,
literal|"renamed:    %s -> %s"
argument_list|,
name|one
argument_list|,
name|two
argument_list|)
expr_stmt|;
break|break;
case|case
name|DIFF_STATUS_TYPE_CHANGED
case|:
name|color_fprintf
argument_list|(
name|s
operator|->
name|fp
argument_list|,
name|c
argument_list|,
literal|"typechange: %s"
argument_list|,
name|one
argument_list|)
expr_stmt|;
break|break;
case|case
name|DIFF_STATUS_UNKNOWN
case|:
name|color_fprintf
argument_list|(
name|s
operator|->
name|fp
argument_list|,
name|c
argument_list|,
literal|"unknown:    %s"
argument_list|,
name|one
argument_list|)
expr_stmt|;
break|break;
case|case
name|DIFF_STATUS_UNMERGED
case|:
name|color_fprintf
argument_list|(
name|s
operator|->
name|fp
argument_list|,
name|c
argument_list|,
literal|"unmerged:   %s"
argument_list|,
name|one
argument_list|)
expr_stmt|;
break|break;
default|default:
name|die
argument_list|(
literal|"bug: unhandled diff status %c"
argument_list|,
name|p
operator|->
name|status
argument_list|)
expr_stmt|;
block|}
name|fprintf
argument_list|(
name|s
operator|->
name|fp
argument_list|,
literal|"\n"
argument_list|)
expr_stmt|;
name|strbuf_release
argument_list|(
operator|&
name|onebuf
argument_list|)
expr_stmt|;
name|strbuf_release
argument_list|(
operator|&
name|twobuf
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|wt_status_print_updated_cb
specifier|static
name|void
name|wt_status_print_updated_cb
parameter_list|(
name|struct
name|diff_queue_struct
modifier|*
name|q
parameter_list|,
name|struct
name|diff_options
modifier|*
name|options
parameter_list|,
name|void
modifier|*
name|data
parameter_list|)
block|{
name|struct
name|wt_status
modifier|*
name|s
init|=
name|data
decl_stmt|;
name|int
name|shown_header
init|=
literal|0
decl_stmt|;
name|int
name|i
decl_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|q
operator|->
name|nr
condition|;
name|i
operator|++
control|)
block|{
if|if
condition|(
name|q
operator|->
name|queue
index|[
name|i
index|]
operator|->
name|status
operator|==
literal|'U'
condition|)
continue|continue;
if|if
condition|(
operator|!
name|shown_header
condition|)
block|{
name|wt_status_print_cached_header
argument_list|(
name|s
argument_list|)
expr_stmt|;
name|s
operator|->
name|commitable
operator|=
literal|1
expr_stmt|;
name|shown_header
operator|=
literal|1
expr_stmt|;
block|}
name|wt_status_print_filepair
argument_list|(
name|s
argument_list|,
name|WT_STATUS_UPDATED
argument_list|,
name|q
operator|->
name|queue
index|[
name|i
index|]
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|shown_header
condition|)
name|wt_status_print_trailer
argument_list|(
name|s
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|wt_status_print_changed_cb
specifier|static
name|void
name|wt_status_print_changed_cb
parameter_list|(
name|struct
name|diff_queue_struct
modifier|*
name|q
parameter_list|,
name|struct
name|diff_options
modifier|*
name|options
parameter_list|,
name|void
modifier|*
name|data
parameter_list|)
block|{
name|struct
name|wt_status
modifier|*
name|s
init|=
name|data
decl_stmt|;
name|int
name|i
decl_stmt|;
if|if
condition|(
name|q
operator|->
name|nr
condition|)
block|{
name|int
name|has_deleted
init|=
literal|0
decl_stmt|;
name|s
operator|->
name|workdir_dirty
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
name|q
operator|->
name|nr
condition|;
name|i
operator|++
control|)
if|if
condition|(
name|q
operator|->
name|queue
index|[
name|i
index|]
operator|->
name|status
operator|==
name|DIFF_STATUS_DELETED
condition|)
block|{
name|has_deleted
operator|=
literal|1
expr_stmt|;
break|break;
block|}
name|wt_status_print_dirty_header
argument_list|(
name|s
argument_list|,
name|has_deleted
argument_list|)
expr_stmt|;
block|}
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|q
operator|->
name|nr
condition|;
name|i
operator|++
control|)
name|wt_status_print_filepair
argument_list|(
name|s
argument_list|,
name|WT_STATUS_CHANGED
argument_list|,
name|q
operator|->
name|queue
index|[
name|i
index|]
argument_list|)
expr_stmt|;
if|if
condition|(
name|q
operator|->
name|nr
condition|)
name|wt_status_print_trailer
argument_list|(
name|s
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|wt_status_print_updated
specifier|static
name|void
name|wt_status_print_updated
parameter_list|(
name|struct
name|wt_status
modifier|*
name|s
parameter_list|)
block|{
name|struct
name|rev_info
name|rev
decl_stmt|;
name|init_revisions
argument_list|(
operator|&
name|rev
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|setup_revisions
argument_list|(
literal|0
argument_list|,
name|NULL
argument_list|,
operator|&
name|rev
argument_list|,
name|s
operator|->
name|is_initial
condition|?
name|EMPTY_TREE_SHA1_HEX
else|:
name|s
operator|->
name|reference
argument_list|)
expr_stmt|;
name|rev
operator|.
name|diffopt
operator|.
name|output_format
operator||=
name|DIFF_FORMAT_CALLBACK
expr_stmt|;
name|rev
operator|.
name|diffopt
operator|.
name|format_callback
operator|=
name|wt_status_print_updated_cb
expr_stmt|;
name|rev
operator|.
name|diffopt
operator|.
name|format_callback_data
operator|=
name|s
expr_stmt|;
name|rev
operator|.
name|diffopt
operator|.
name|detect_rename
operator|=
literal|1
expr_stmt|;
name|rev
operator|.
name|diffopt
operator|.
name|rename_limit
operator|=
literal|200
expr_stmt|;
name|rev
operator|.
name|diffopt
operator|.
name|break_opt
operator|=
literal|0
expr_stmt|;
name|run_diff_index
argument_list|(
operator|&
name|rev
argument_list|,
literal|1
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|wt_status_print_changed
specifier|static
name|void
name|wt_status_print_changed
parameter_list|(
name|struct
name|wt_status
modifier|*
name|s
parameter_list|)
block|{
name|struct
name|rev_info
name|rev
decl_stmt|;
name|init_revisions
argument_list|(
operator|&
name|rev
argument_list|,
literal|""
argument_list|)
expr_stmt|;
name|setup_revisions
argument_list|(
literal|0
argument_list|,
name|NULL
argument_list|,
operator|&
name|rev
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|rev
operator|.
name|diffopt
operator|.
name|output_format
operator||=
name|DIFF_FORMAT_CALLBACK
expr_stmt|;
name|rev
operator|.
name|diffopt
operator|.
name|format_callback
operator|=
name|wt_status_print_changed_cb
expr_stmt|;
name|rev
operator|.
name|diffopt
operator|.
name|format_callback_data
operator|=
name|s
expr_stmt|;
name|run_diff_files
argument_list|(
operator|&
name|rev
argument_list|,
literal|0
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|wt_status_print_submodule_summary
specifier|static
name|void
name|wt_status_print_submodule_summary
parameter_list|(
name|struct
name|wt_status
modifier|*
name|s
parameter_list|)
block|{
name|struct
name|child_process
name|sm_summary
decl_stmt|;
name|char
name|summary_limit
index|[
literal|64
index|]
decl_stmt|;
name|char
name|index
index|[
name|PATH_MAX
index|]
decl_stmt|;
specifier|const
name|char
modifier|*
name|env
index|[]
init|=
block|{
name|index
block|,
name|NULL
block|}
decl_stmt|;
specifier|const
name|char
modifier|*
name|argv
index|[]
init|=
block|{
literal|"submodule"
block|,
literal|"summary"
block|,
literal|"--cached"
block|,
literal|"--for-status"
block|,
literal|"--summary-limit"
block|,
name|summary_limit
block|,
name|s
operator|->
name|amend
operator|?
literal|"HEAD^"
operator|:
literal|"HEAD"
block|,
name|NULL
block|}
decl_stmt|;
name|sprintf
argument_list|(
name|summary_limit
argument_list|,
literal|"%d"
argument_list|,
name|wt_status_submodule_summary
argument_list|)
expr_stmt|;
name|snprintf
argument_list|(
name|index
argument_list|,
sizeof|sizeof
argument_list|(
name|index
argument_list|)
argument_list|,
literal|"GIT_INDEX_FILE=%s"
argument_list|,
name|s
operator|->
name|index_file
argument_list|)
expr_stmt|;
name|memset
argument_list|(
operator|&
name|sm_summary
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
name|sm_summary
argument_list|)
argument_list|)
expr_stmt|;
name|sm_summary
operator|.
name|argv
operator|=
name|argv
expr_stmt|;
name|sm_summary
operator|.
name|env
operator|=
name|env
expr_stmt|;
name|sm_summary
operator|.
name|git_cmd
operator|=
literal|1
expr_stmt|;
name|sm_summary
operator|.
name|no_stdin
operator|=
literal|1
expr_stmt|;
name|fflush
argument_list|(
name|s
operator|->
name|fp
argument_list|)
expr_stmt|;
name|sm_summary
operator|.
name|out
operator|=
name|dup
argument_list|(
name|fileno
argument_list|(
name|s
operator|->
name|fp
argument_list|)
argument_list|)
expr_stmt|;
comment|/* run_command closes it */
name|run_command
argument_list|(
operator|&
name|sm_summary
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|wt_status_print_untracked
specifier|static
name|void
name|wt_status_print_untracked
parameter_list|(
name|struct
name|wt_status
modifier|*
name|s
parameter_list|)
block|{
name|struct
name|dir_struct
name|dir
decl_stmt|;
name|int
name|i
decl_stmt|;
name|int
name|shown_header
init|=
literal|0
decl_stmt|;
name|struct
name|strbuf
name|buf
init|=
name|STRBUF_INIT
decl_stmt|;
name|memset
argument_list|(
operator|&
name|dir
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
name|dir
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|s
operator|->
name|untracked
condition|)
block|{
name|dir
operator|.
name|show_other_directories
operator|=
literal|1
expr_stmt|;
name|dir
operator|.
name|hide_empty_directories
operator|=
literal|1
expr_stmt|;
block|}
name|setup_standard_excludes
argument_list|(
operator|&
name|dir
argument_list|)
expr_stmt|;
name|read_directory
argument_list|(
operator|&
name|dir
argument_list|,
literal|"."
argument_list|,
literal|""
argument_list|,
literal|0
argument_list|,
name|NULL
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
name|dir
operator|.
name|nr
condition|;
name|i
operator|++
control|)
block|{
name|struct
name|dir_entry
modifier|*
name|ent
init|=
name|dir
operator|.
name|entries
index|[
name|i
index|]
decl_stmt|;
if|if
condition|(
operator|!
name|cache_name_is_other
argument_list|(
name|ent
operator|->
name|name
argument_list|,
name|ent
operator|->
name|len
argument_list|)
condition|)
continue|continue;
if|if
condition|(
operator|!
name|shown_header
condition|)
block|{
name|s
operator|->
name|workdir_untracked
operator|=
literal|1
expr_stmt|;
name|wt_status_print_untracked_header
argument_list|(
name|s
argument_list|)
expr_stmt|;
name|shown_header
operator|=
literal|1
expr_stmt|;
block|}
name|color_fprintf
argument_list|(
name|s
operator|->
name|fp
argument_list|,
name|color
argument_list|(
name|WT_STATUS_HEADER
argument_list|)
argument_list|,
literal|"#\t"
argument_list|)
expr_stmt|;
name|color_fprintf_ln
argument_list|(
name|s
operator|->
name|fp
argument_list|,
name|color
argument_list|(
name|WT_STATUS_UNTRACKED
argument_list|)
argument_list|,
literal|"%s"
argument_list|,
name|quote_path
argument_list|(
name|ent
operator|->
name|name
argument_list|,
name|ent
operator|->
name|len
argument_list|,
operator|&
name|buf
argument_list|,
name|s
operator|->
name|prefix
argument_list|)
argument_list|)
expr_stmt|;
block|}
name|strbuf_release
argument_list|(
operator|&
name|buf
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|wt_status_print_verbose
specifier|static
name|void
name|wt_status_print_verbose
parameter_list|(
name|struct
name|wt_status
modifier|*
name|s
parameter_list|)
block|{
name|struct
name|rev_info
name|rev
decl_stmt|;
name|init_revisions
argument_list|(
operator|&
name|rev
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|DIFF_OPT_SET
argument_list|(
operator|&
name|rev
operator|.
name|diffopt
argument_list|,
name|ALLOW_TEXTCONV
argument_list|)
expr_stmt|;
name|setup_revisions
argument_list|(
literal|0
argument_list|,
name|NULL
argument_list|,
operator|&
name|rev
argument_list|,
name|s
operator|->
name|is_initial
condition|?
name|EMPTY_TREE_SHA1_HEX
else|:
name|s
operator|->
name|reference
argument_list|)
expr_stmt|;
name|rev
operator|.
name|diffopt
operator|.
name|output_format
operator||=
name|DIFF_FORMAT_PATCH
expr_stmt|;
name|rev
operator|.
name|diffopt
operator|.
name|detect_rename
operator|=
literal|1
expr_stmt|;
name|rev
operator|.
name|diffopt
operator|.
name|file
operator|=
name|s
operator|->
name|fp
expr_stmt|;
name|rev
operator|.
name|diffopt
operator|.
name|close_file
operator|=
literal|0
expr_stmt|;
comment|/* 	 * If we're not going to stdout, then we definitely don't 	 * want color, since we are going to the commit message 	 * file (and even the "auto" setting won't work, since it 	 * will have checked isatty on stdout). 	 */
if|if
condition|(
name|s
operator|->
name|fp
operator|!=
name|stdout
condition|)
name|DIFF_OPT_CLR
argument_list|(
operator|&
name|rev
operator|.
name|diffopt
argument_list|,
name|COLOR_DIFF
argument_list|)
expr_stmt|;
name|run_diff_index
argument_list|(
operator|&
name|rev
argument_list|,
literal|1
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|wt_status_print_tracking
specifier|static
name|void
name|wt_status_print_tracking
parameter_list|(
name|struct
name|wt_status
modifier|*
name|s
parameter_list|)
block|{
name|struct
name|strbuf
name|sb
init|=
name|STRBUF_INIT
decl_stmt|;
specifier|const
name|char
modifier|*
name|cp
decl_stmt|,
modifier|*
name|ep
decl_stmt|;
name|struct
name|branch
modifier|*
name|branch
decl_stmt|;
name|assert
argument_list|(
name|s
operator|->
name|branch
operator|&&
operator|!
name|s
operator|->
name|is_initial
argument_list|)
expr_stmt|;
if|if
condition|(
name|prefixcmp
argument_list|(
name|s
operator|->
name|branch
argument_list|,
literal|"refs/heads/"
argument_list|)
condition|)
return|return;
name|branch
operator|=
name|branch_get
argument_list|(
name|s
operator|->
name|branch
operator|+
literal|11
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|format_tracking_info
argument_list|(
name|branch
argument_list|,
operator|&
name|sb
argument_list|)
condition|)
return|return;
for|for
control|(
name|cp
operator|=
name|sb
operator|.
name|buf
init|;
operator|(
name|ep
operator|=
name|strchr
argument_list|(
name|cp
argument_list|,
literal|'\n'
argument_list|)
operator|)
operator|!=
name|NULL
condition|;
name|cp
operator|=
name|ep
operator|+
literal|1
control|)
name|color_fprintf_ln
argument_list|(
name|s
operator|->
name|fp
argument_list|,
name|color
argument_list|(
name|WT_STATUS_HEADER
argument_list|)
argument_list|,
literal|"# %.*s"
argument_list|,
call|(
name|int
call|)
argument_list|(
name|ep
operator|-
name|cp
argument_list|)
argument_list|,
name|cp
argument_list|)
expr_stmt|;
name|color_fprintf_ln
argument_list|(
name|s
operator|->
name|fp
argument_list|,
name|color
argument_list|(
name|WT_STATUS_HEADER
argument_list|)
argument_list|,
literal|"#"
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|wt_status_print
name|void
name|wt_status_print
parameter_list|(
name|struct
name|wt_status
modifier|*
name|s
parameter_list|)
block|{
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
specifier|const
name|char
modifier|*
name|branch_color
init|=
name|color
argument_list|(
name|WT_STATUS_HEADER
argument_list|)
decl_stmt|;
name|s
operator|->
name|is_initial
operator|=
name|get_sha1
argument_list|(
name|s
operator|->
name|reference
argument_list|,
name|sha1
argument_list|)
condition|?
literal|1
else|:
literal|0
expr_stmt|;
if|if
condition|(
name|s
operator|->
name|branch
condition|)
block|{
specifier|const
name|char
modifier|*
name|on_what
init|=
literal|"On branch "
decl_stmt|;
specifier|const
name|char
modifier|*
name|branch_name
init|=
name|s
operator|->
name|branch
decl_stmt|;
if|if
condition|(
operator|!
name|prefixcmp
argument_list|(
name|branch_name
argument_list|,
literal|"refs/heads/"
argument_list|)
condition|)
name|branch_name
operator|+=
literal|11
expr_stmt|;
elseif|else
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|branch_name
argument_list|,
literal|"HEAD"
argument_list|)
condition|)
block|{
name|branch_name
operator|=
literal|""
expr_stmt|;
name|branch_color
operator|=
name|color
argument_list|(
name|WT_STATUS_NOBRANCH
argument_list|)
expr_stmt|;
name|on_what
operator|=
literal|"Not currently on any branch."
expr_stmt|;
block|}
name|color_fprintf
argument_list|(
name|s
operator|->
name|fp
argument_list|,
name|color
argument_list|(
name|WT_STATUS_HEADER
argument_list|)
argument_list|,
literal|"# "
argument_list|)
expr_stmt|;
name|color_fprintf_ln
argument_list|(
name|s
operator|->
name|fp
argument_list|,
name|branch_color
argument_list|,
literal|"%s%s"
argument_list|,
name|on_what
argument_list|,
name|branch_name
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|s
operator|->
name|is_initial
condition|)
name|wt_status_print_tracking
argument_list|(
name|s
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|s
operator|->
name|is_initial
condition|)
block|{
name|color_fprintf_ln
argument_list|(
name|s
operator|->
name|fp
argument_list|,
name|color
argument_list|(
name|WT_STATUS_HEADER
argument_list|)
argument_list|,
literal|"#"
argument_list|)
expr_stmt|;
name|color_fprintf_ln
argument_list|(
name|s
operator|->
name|fp
argument_list|,
name|color
argument_list|(
name|WT_STATUS_HEADER
argument_list|)
argument_list|,
literal|"# Initial commit"
argument_list|)
expr_stmt|;
name|color_fprintf_ln
argument_list|(
name|s
operator|->
name|fp
argument_list|,
name|color
argument_list|(
name|WT_STATUS_HEADER
argument_list|)
argument_list|,
literal|"#"
argument_list|)
expr_stmt|;
block|}
name|wt_status_print_updated
argument_list|(
name|s
argument_list|)
expr_stmt|;
name|wt_status_print_changed
argument_list|(
name|s
argument_list|)
expr_stmt|;
if|if
condition|(
name|wt_status_submodule_summary
condition|)
name|wt_status_print_submodule_summary
argument_list|(
name|s
argument_list|)
expr_stmt|;
if|if
condition|(
name|show_untracked_files
condition|)
name|wt_status_print_untracked
argument_list|(
name|s
argument_list|)
expr_stmt|;
elseif|else
if|if
condition|(
name|s
operator|->
name|commitable
condition|)
name|fprintf
argument_list|(
name|s
operator|->
name|fp
argument_list|,
literal|"# Untracked files not listed (use -u option to show untracked files)\n"
argument_list|)
expr_stmt|;
if|if
condition|(
name|s
operator|->
name|verbose
condition|)
name|wt_status_print_verbose
argument_list|(
name|s
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|s
operator|->
name|commitable
condition|)
block|{
if|if
condition|(
name|s
operator|->
name|amend
condition|)
name|fprintf
argument_list|(
name|s
operator|->
name|fp
argument_list|,
literal|"# No changes\n"
argument_list|)
expr_stmt|;
elseif|else
if|if
condition|(
name|s
operator|->
name|nowarn
condition|)
empty_stmt|;
comment|/* nothing */
elseif|else
if|if
condition|(
name|s
operator|->
name|workdir_dirty
condition|)
name|printf
argument_list|(
literal|"no changes added to commit (use \"git add\" and/or \"git commit -a\")\n"
argument_list|)
expr_stmt|;
elseif|else
if|if
condition|(
name|s
operator|->
name|workdir_untracked
condition|)
name|printf
argument_list|(
literal|"nothing added to commit but untracked files present (use \"git add\" to track)\n"
argument_list|)
expr_stmt|;
elseif|else
if|if
condition|(
name|s
operator|->
name|is_initial
condition|)
name|printf
argument_list|(
literal|"nothing to commit (create/copy files and use \"git add\" to track)\n"
argument_list|)
expr_stmt|;
elseif|else
if|if
condition|(
operator|!
name|show_untracked_files
condition|)
name|printf
argument_list|(
literal|"nothing to commit (use -u to show untracked files)\n"
argument_list|)
expr_stmt|;
else|else
name|printf
argument_list|(
literal|"nothing to commit (working directory clean)\n"
argument_list|)
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|git_status_config
name|int
name|git_status_config
parameter_list|(
specifier|const
name|char
modifier|*
name|k
parameter_list|,
specifier|const
name|char
modifier|*
name|v
parameter_list|,
name|void
modifier|*
name|cb
parameter_list|)
block|{
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|k
argument_list|,
literal|"status.submodulesummary"
argument_list|)
condition|)
block|{
name|int
name|is_bool
decl_stmt|;
name|wt_status_submodule_summary
operator|=
name|git_config_bool_or_int
argument_list|(
name|k
argument_list|,
name|v
argument_list|,
operator|&
name|is_bool
argument_list|)
expr_stmt|;
if|if
condition|(
name|is_bool
operator|&&
name|wt_status_submodule_summary
condition|)
name|wt_status_submodule_summary
operator|=
operator|-
literal|1
expr_stmt|;
return|return
literal|0
return|;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|k
argument_list|,
literal|"status.color"
argument_list|)
operator|||
operator|!
name|strcmp
argument_list|(
name|k
argument_list|,
literal|"color.status"
argument_list|)
condition|)
block|{
name|wt_status_use_color
operator|=
name|git_config_colorbool
argument_list|(
name|k
argument_list|,
name|v
argument_list|,
operator|-
literal|1
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
if|if
condition|(
operator|!
name|prefixcmp
argument_list|(
name|k
argument_list|,
literal|"status.color."
argument_list|)
operator|||
operator|!
name|prefixcmp
argument_list|(
name|k
argument_list|,
literal|"color.status."
argument_list|)
condition|)
block|{
name|int
name|slot
init|=
name|parse_status_slot
argument_list|(
name|k
argument_list|,
literal|13
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
name|v
condition|)
return|return
name|config_error_nonbool
argument_list|(
name|k
argument_list|)
return|;
name|color_parse
argument_list|(
name|v
argument_list|,
name|k
argument_list|,
name|wt_status_colors
index|[
name|slot
index|]
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|k
argument_list|,
literal|"status.relativepaths"
argument_list|)
condition|)
block|{
name|wt_status_relative_paths
operator|=
name|git_config_bool
argument_list|(
name|k
argument_list|,
name|v
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|k
argument_list|,
literal|"status.showuntrackedfiles"
argument_list|)
condition|)
block|{
if|if
condition|(
operator|!
name|v
condition|)
return|return
name|config_error_nonbool
argument_list|(
name|k
argument_list|)
return|;
elseif|else
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|v
argument_list|,
literal|"no"
argument_list|)
condition|)
name|show_untracked_files
operator|=
name|SHOW_NO_UNTRACKED_FILES
expr_stmt|;
elseif|else
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|v
argument_list|,
literal|"normal"
argument_list|)
condition|)
name|show_untracked_files
operator|=
name|SHOW_NORMAL_UNTRACKED_FILES
expr_stmt|;
elseif|else
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|v
argument_list|,
literal|"all"
argument_list|)
condition|)
name|show_untracked_files
operator|=
name|SHOW_ALL_UNTRACKED_FILES
expr_stmt|;
else|else
return|return
name|error
argument_list|(
literal|"Invalid untracked files mode '%s'"
argument_list|,
name|v
argument_list|)
return|;
return|return
literal|0
return|;
block|}
return|return
name|git_diff_ui_config
argument_list|(
name|k
argument_list|,
name|v
argument_list|,
name|cb
argument_list|)
return|;
block|}
end_function
end_unit
