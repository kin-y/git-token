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
file|"parse-options.h"
end_include
begin_include
include|#
directive|include
file|"quote.h"
end_include
begin_include
include|#
directive|include
file|"pathspec.h"
end_include
begin_include
include|#
directive|include
file|"dir.h"
end_include
begin_include
include|#
directive|include
file|"utf8.h"
end_include
begin_struct
DECL|struct|module_list
struct|struct
name|module_list
block|{
DECL|member|entries
specifier|const
name|struct
name|cache_entry
modifier|*
modifier|*
name|entries
decl_stmt|;
DECL|member|alloc
DECL|member|nr
name|int
name|alloc
decl_stmt|,
name|nr
decl_stmt|;
block|}
struct|;
end_struct
begin_define
DECL|macro|MODULE_LIST_INIT
define|#
directive|define
name|MODULE_LIST_INIT
value|{ NULL, 0, 0 }
end_define
begin_function
DECL|function|module_list_compute
specifier|static
name|int
name|module_list_compute
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
parameter_list|,
name|struct
name|pathspec
modifier|*
name|pathspec
parameter_list|,
name|struct
name|module_list
modifier|*
name|list
parameter_list|)
block|{
name|int
name|i
decl_stmt|,
name|result
init|=
literal|0
decl_stmt|;
name|char
modifier|*
name|max_prefix
decl_stmt|,
modifier|*
name|ps_matched
init|=
name|NULL
decl_stmt|;
name|int
name|max_prefix_len
decl_stmt|;
name|parse_pathspec
argument_list|(
name|pathspec
argument_list|,
literal|0
argument_list|,
name|PATHSPEC_PREFER_FULL
operator||
name|PATHSPEC_STRIP_SUBMODULE_SLASH_CHEAP
argument_list|,
name|prefix
argument_list|,
name|argv
argument_list|)
expr_stmt|;
comment|/* Find common prefix for all pathspec's */
name|max_prefix
operator|=
name|common_prefix
argument_list|(
name|pathspec
argument_list|)
expr_stmt|;
name|max_prefix_len
operator|=
name|max_prefix
condition|?
name|strlen
argument_list|(
name|max_prefix
argument_list|)
else|:
literal|0
expr_stmt|;
if|if
condition|(
name|pathspec
operator|->
name|nr
condition|)
name|ps_matched
operator|=
name|xcalloc
argument_list|(
name|pathspec
operator|->
name|nr
argument_list|,
literal|1
argument_list|)
expr_stmt|;
if|if
condition|(
name|read_cache
argument_list|()
operator|<
literal|0
condition|)
name|die
argument_list|(
name|_
argument_list|(
literal|"index file corrupt"
argument_list|)
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
name|active_nr
condition|;
name|i
operator|++
control|)
block|{
specifier|const
name|struct
name|cache_entry
modifier|*
name|ce
init|=
name|active_cache
index|[
name|i
index|]
decl_stmt|;
if|if
condition|(
operator|!
name|S_ISGITLINK
argument_list|(
name|ce
operator|->
name|ce_mode
argument_list|)
operator|||
operator|!
name|match_pathspec
argument_list|(
name|pathspec
argument_list|,
name|ce
operator|->
name|name
argument_list|,
name|ce_namelen
argument_list|(
name|ce
argument_list|)
argument_list|,
name|max_prefix_len
argument_list|,
name|ps_matched
argument_list|,
literal|1
argument_list|)
condition|)
continue|continue;
name|ALLOC_GROW
argument_list|(
name|list
operator|->
name|entries
argument_list|,
name|list
operator|->
name|nr
operator|+
literal|1
argument_list|,
name|list
operator|->
name|alloc
argument_list|)
expr_stmt|;
name|list
operator|->
name|entries
index|[
name|list
operator|->
name|nr
operator|++
index|]
operator|=
name|ce
expr_stmt|;
while|while
condition|(
name|i
operator|+
literal|1
operator|<
name|active_nr
operator|&&
operator|!
name|strcmp
argument_list|(
name|ce
operator|->
name|name
argument_list|,
name|active_cache
index|[
name|i
operator|+
literal|1
index|]
operator|->
name|name
argument_list|)
condition|)
comment|/* 			 * Skip entries with the same name in different stages 			 * to make sure an entry is returned only once. 			 */
name|i
operator|++
expr_stmt|;
block|}
name|free
argument_list|(
name|max_prefix
argument_list|)
expr_stmt|;
if|if
condition|(
name|ps_matched
operator|&&
name|report_path_error
argument_list|(
name|ps_matched
argument_list|,
name|pathspec
argument_list|,
name|prefix
argument_list|)
condition|)
name|result
operator|=
operator|-
literal|1
expr_stmt|;
name|free
argument_list|(
name|ps_matched
argument_list|)
expr_stmt|;
return|return
name|result
return|;
block|}
end_function
begin_function
DECL|function|module_list
specifier|static
name|int
name|module_list
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
name|int
name|i
decl_stmt|;
name|struct
name|pathspec
name|pathspec
decl_stmt|;
name|struct
name|module_list
name|list
init|=
name|MODULE_LIST_INIT
decl_stmt|;
name|struct
name|option
name|module_list_options
index|[]
init|=
block|{
name|OPT_STRING
argument_list|(
literal|0
argument_list|,
literal|"prefix"
argument_list|,
operator|&
name|prefix
argument_list|,
name|N_
argument_list|(
literal|"path"
argument_list|)
argument_list|,
name|N_
argument_list|(
literal|"alternative anchor for relative paths"
argument_list|)
argument_list|)
block|,
name|OPT_END
argument_list|()
block|}
decl_stmt|;
specifier|const
name|char
modifier|*
specifier|const
name|git_submodule_helper_usage
index|[]
init|=
block|{
name|N_
argument_list|(
literal|"git submodule--helper list [--prefix=<path>] [<path>...]"
argument_list|)
block|,
name|NULL
block|}
decl_stmt|;
name|argc
operator|=
name|parse_options
argument_list|(
name|argc
argument_list|,
name|argv
argument_list|,
name|prefix
argument_list|,
name|module_list_options
argument_list|,
name|git_submodule_helper_usage
argument_list|,
literal|0
argument_list|)
expr_stmt|;
if|if
condition|(
name|module_list_compute
argument_list|(
name|argc
argument_list|,
name|argv
argument_list|,
name|prefix
argument_list|,
operator|&
name|pathspec
argument_list|,
operator|&
name|list
argument_list|)
operator|<
literal|0
condition|)
block|{
name|printf
argument_list|(
literal|"#unmatched\n"
argument_list|)
expr_stmt|;
return|return
literal|1
return|;
block|}
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|list
operator|.
name|nr
condition|;
name|i
operator|++
control|)
block|{
specifier|const
name|struct
name|cache_entry
modifier|*
name|ce
init|=
name|list
operator|.
name|entries
index|[
name|i
index|]
decl_stmt|;
if|if
condition|(
name|ce_stage
argument_list|(
name|ce
argument_list|)
condition|)
name|printf
argument_list|(
literal|"%06o %s U\t"
argument_list|,
name|ce
operator|->
name|ce_mode
argument_list|,
name|sha1_to_hex
argument_list|(
name|null_sha1
argument_list|)
argument_list|)
expr_stmt|;
else|else
name|printf
argument_list|(
literal|"%06o %s %d\t"
argument_list|,
name|ce
operator|->
name|ce_mode
argument_list|,
name|sha1_to_hex
argument_list|(
name|ce
operator|->
name|sha1
argument_list|)
argument_list|,
name|ce_stage
argument_list|(
name|ce
argument_list|)
argument_list|)
expr_stmt|;
name|utf8_fprintf
argument_list|(
name|stdout
argument_list|,
literal|"%s\n"
argument_list|,
name|ce
operator|->
name|name
argument_list|)
expr_stmt|;
block|}
return|return
literal|0
return|;
block|}
end_function
begin_struct
DECL|struct|cmd_struct
struct|struct
name|cmd_struct
block|{
DECL|member|cmd
specifier|const
name|char
modifier|*
name|cmd
decl_stmt|;
DECL|member|fn
name|int
function_decl|(
modifier|*
name|fn
function_decl|)
parameter_list|(
name|int
parameter_list|,
specifier|const
name|char
modifier|*
modifier|*
parameter_list|,
specifier|const
name|char
modifier|*
parameter_list|)
function_decl|;
block|}
struct|;
end_struct
begin_decl_stmt
DECL|variable|commands
specifier|static
name|struct
name|cmd_struct
name|commands
index|[]
init|=
block|{
block|{
literal|"list"
block|,
name|module_list
block|}
block|, }
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|cmd_submodule__helper
name|int
name|cmd_submodule__helper
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
name|int
name|i
decl_stmt|;
if|if
condition|(
name|argc
operator|<
literal|2
condition|)
name|die
argument_list|(
name|_
argument_list|(
literal|"fatal: submodule--helper subcommand must be "
literal|"called with a subcommand"
argument_list|)
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
name|ARRAY_SIZE
argument_list|(
name|commands
argument_list|)
condition|;
name|i
operator|++
control|)
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|argv
index|[
literal|1
index|]
argument_list|,
name|commands
index|[
name|i
index|]
operator|.
name|cmd
argument_list|)
condition|)
return|return
name|commands
index|[
name|i
index|]
operator|.
name|fn
argument_list|(
name|argc
operator|-
literal|1
argument_list|,
name|argv
operator|+
literal|1
argument_list|,
name|prefix
argument_list|)
return|;
name|die
argument_list|(
name|_
argument_list|(
literal|"fatal: '%s' is not a valid submodule--helper "
literal|"subcommand"
argument_list|)
argument_list|,
name|argv
index|[
literal|1
index|]
argument_list|)
expr_stmt|;
block|}
end_function
end_unit
