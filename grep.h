begin_unit
begin_ifndef
ifndef|#
directive|ifndef
name|GREP_H
end_ifndef
begin_define
DECL|macro|GREP_H
define|#
directive|define
name|GREP_H
end_define
begin_include
include|#
directive|include
file|"color.h"
end_include
begin_ifdef
ifdef|#
directive|ifdef
name|USE_LIBPCRE
end_ifdef
begin_include
include|#
directive|include
file|<pcre.h>
end_include
begin_else
else|#
directive|else
end_else
begin_typedef
DECL|typedef|pcre
typedef|typedef
name|int
name|pcre
typedef|;
end_typedef
begin_typedef
DECL|typedef|pcre_extra
typedef|typedef
name|int
name|pcre_extra
typedef|;
end_typedef
begin_endif
endif|#
directive|endif
end_endif
begin_include
include|#
directive|include
file|"kwset.h"
end_include
begin_include
include|#
directive|include
file|"thread-utils.h"
end_include
begin_include
include|#
directive|include
file|"userdiff.h"
end_include
begin_enum
DECL|enum|grep_pat_token
enum|enum
name|grep_pat_token
block|{
DECL|enumerator|GREP_PATTERN
name|GREP_PATTERN
block|,
DECL|enumerator|GREP_PATTERN_HEAD
name|GREP_PATTERN_HEAD
block|,
DECL|enumerator|GREP_PATTERN_BODY
name|GREP_PATTERN_BODY
block|,
DECL|enumerator|GREP_AND
name|GREP_AND
block|,
DECL|enumerator|GREP_OPEN_PAREN
name|GREP_OPEN_PAREN
block|,
DECL|enumerator|GREP_CLOSE_PAREN
name|GREP_CLOSE_PAREN
block|,
DECL|enumerator|GREP_NOT
name|GREP_NOT
block|,
DECL|enumerator|GREP_OR
name|GREP_OR
block|}
enum|;
end_enum
begin_enum
DECL|enum|grep_context
enum|enum
name|grep_context
block|{
DECL|enumerator|GREP_CONTEXT_HEAD
name|GREP_CONTEXT_HEAD
block|,
DECL|enumerator|GREP_CONTEXT_BODY
name|GREP_CONTEXT_BODY
block|}
enum|;
end_enum
begin_enum
DECL|enum|grep_header_field
enum|enum
name|grep_header_field
block|{
DECL|enumerator|GREP_HEADER_FIELD_MIN
name|GREP_HEADER_FIELD_MIN
init|=
literal|0
block|,
DECL|enumerator|GREP_HEADER_AUTHOR
name|GREP_HEADER_AUTHOR
init|=
name|GREP_HEADER_FIELD_MIN
block|,
DECL|enumerator|GREP_HEADER_COMMITTER
name|GREP_HEADER_COMMITTER
block|,
DECL|enumerator|GREP_HEADER_REFLOG
name|GREP_HEADER_REFLOG
block|,
comment|/* Must be at the end of the enum */
DECL|enumerator|GREP_HEADER_FIELD_MAX
name|GREP_HEADER_FIELD_MAX
block|}
enum|;
end_enum
begin_struct
DECL|struct|grep_pat
struct|struct
name|grep_pat
block|{
DECL|member|next
name|struct
name|grep_pat
modifier|*
name|next
decl_stmt|;
DECL|member|origin
specifier|const
name|char
modifier|*
name|origin
decl_stmt|;
DECL|member|no
name|int
name|no
decl_stmt|;
DECL|member|token
name|enum
name|grep_pat_token
name|token
decl_stmt|;
DECL|member|pattern
name|char
modifier|*
name|pattern
decl_stmt|;
DECL|member|patternlen
name|size_t
name|patternlen
decl_stmt|;
DECL|member|field
name|enum
name|grep_header_field
name|field
decl_stmt|;
DECL|member|regexp
name|regex_t
name|regexp
decl_stmt|;
DECL|member|pcre_regexp
name|pcre
modifier|*
name|pcre_regexp
decl_stmt|;
DECL|member|pcre_extra_info
name|pcre_extra
modifier|*
name|pcre_extra_info
decl_stmt|;
DECL|member|kws
name|kwset_t
name|kws
decl_stmt|;
DECL|member|fixed
name|unsigned
name|fixed
range|:
literal|1
decl_stmt|;
DECL|member|ignore_case
name|unsigned
name|ignore_case
range|:
literal|1
decl_stmt|;
DECL|member|word_regexp
name|unsigned
name|word_regexp
range|:
literal|1
decl_stmt|;
block|}
struct|;
end_struct
begin_enum
DECL|enum|grep_expr_node
enum|enum
name|grep_expr_node
block|{
DECL|enumerator|GREP_NODE_ATOM
name|GREP_NODE_ATOM
block|,
DECL|enumerator|GREP_NODE_NOT
name|GREP_NODE_NOT
block|,
DECL|enumerator|GREP_NODE_AND
name|GREP_NODE_AND
block|,
DECL|enumerator|GREP_NODE_TRUE
name|GREP_NODE_TRUE
block|,
DECL|enumerator|GREP_NODE_OR
name|GREP_NODE_OR
block|}
enum|;
end_enum
begin_enum
DECL|enum|grep_pattern_type
enum|enum
name|grep_pattern_type
block|{
DECL|enumerator|GREP_PATTERN_TYPE_UNSPECIFIED
name|GREP_PATTERN_TYPE_UNSPECIFIED
init|=
literal|0
block|,
DECL|enumerator|GREP_PATTERN_TYPE_BRE
name|GREP_PATTERN_TYPE_BRE
block|,
DECL|enumerator|GREP_PATTERN_TYPE_ERE
name|GREP_PATTERN_TYPE_ERE
block|,
DECL|enumerator|GREP_PATTERN_TYPE_FIXED
name|GREP_PATTERN_TYPE_FIXED
block|,
DECL|enumerator|GREP_PATTERN_TYPE_PCRE
name|GREP_PATTERN_TYPE_PCRE
block|}
enum|;
end_enum
begin_struct
DECL|struct|grep_expr
struct|struct
name|grep_expr
block|{
DECL|member|node
name|enum
name|grep_expr_node
name|node
decl_stmt|;
DECL|member|hit
name|unsigned
name|hit
decl_stmt|;
union|union
block|{
DECL|member|atom
name|struct
name|grep_pat
modifier|*
name|atom
decl_stmt|;
DECL|member|unary
name|struct
name|grep_expr
modifier|*
name|unary
decl_stmt|;
struct|struct
block|{
DECL|member|left
name|struct
name|grep_expr
modifier|*
name|left
decl_stmt|;
DECL|member|right
name|struct
name|grep_expr
modifier|*
name|right
decl_stmt|;
block|}
DECL|member|binary
name|binary
struct|;
block|}
DECL|member|u
name|u
union|;
block|}
struct|;
end_struct
begin_struct
DECL|struct|grep_opt
struct|struct
name|grep_opt
block|{
DECL|member|pattern_list
name|struct
name|grep_pat
modifier|*
name|pattern_list
decl_stmt|;
DECL|member|pattern_tail
name|struct
name|grep_pat
modifier|*
modifier|*
name|pattern_tail
decl_stmt|;
DECL|member|header_list
name|struct
name|grep_pat
modifier|*
name|header_list
decl_stmt|;
DECL|member|header_tail
name|struct
name|grep_pat
modifier|*
modifier|*
name|header_tail
decl_stmt|;
DECL|member|pattern_expression
name|struct
name|grep_expr
modifier|*
name|pattern_expression
decl_stmt|;
DECL|member|prefix
specifier|const
name|char
modifier|*
name|prefix
decl_stmt|;
DECL|member|prefix_length
name|int
name|prefix_length
decl_stmt|;
DECL|member|regexp
name|regex_t
name|regexp
decl_stmt|;
DECL|member|linenum
name|int
name|linenum
decl_stmt|;
DECL|member|invert
name|int
name|invert
decl_stmt|;
DECL|member|ignore_case
name|int
name|ignore_case
decl_stmt|;
DECL|member|status_only
name|int
name|status_only
decl_stmt|;
DECL|member|name_only
name|int
name|name_only
decl_stmt|;
DECL|member|unmatch_name_only
name|int
name|unmatch_name_only
decl_stmt|;
DECL|member|count
name|int
name|count
decl_stmt|;
DECL|member|word_regexp
name|int
name|word_regexp
decl_stmt|;
DECL|member|fixed
name|int
name|fixed
decl_stmt|;
DECL|member|all_match
name|int
name|all_match
decl_stmt|;
DECL|member|debug
name|int
name|debug
decl_stmt|;
DECL|macro|GREP_BINARY_DEFAULT
define|#
directive|define
name|GREP_BINARY_DEFAULT
value|0
DECL|macro|GREP_BINARY_NOMATCH
define|#
directive|define
name|GREP_BINARY_NOMATCH
value|1
DECL|macro|GREP_BINARY_TEXT
define|#
directive|define
name|GREP_BINARY_TEXT
value|2
DECL|member|binary
name|int
name|binary
decl_stmt|;
DECL|member|allow_textconv
name|int
name|allow_textconv
decl_stmt|;
DECL|member|extended
name|int
name|extended
decl_stmt|;
DECL|member|use_reflog_filter
name|int
name|use_reflog_filter
decl_stmt|;
DECL|member|pcre
name|int
name|pcre
decl_stmt|;
DECL|member|relative
name|int
name|relative
decl_stmt|;
DECL|member|pathname
name|int
name|pathname
decl_stmt|;
DECL|member|null_following_name
name|int
name|null_following_name
decl_stmt|;
DECL|member|color
name|int
name|color
decl_stmt|;
DECL|member|max_depth
name|int
name|max_depth
decl_stmt|;
DECL|member|funcname
name|int
name|funcname
decl_stmt|;
DECL|member|funcbody
name|int
name|funcbody
decl_stmt|;
DECL|member|extended_regexp_option
name|int
name|extended_regexp_option
decl_stmt|;
DECL|member|pattern_type_option
name|int
name|pattern_type_option
decl_stmt|;
DECL|member|color_context
name|char
name|color_context
index|[
name|COLOR_MAXLEN
index|]
decl_stmt|;
DECL|member|color_filename
name|char
name|color_filename
index|[
name|COLOR_MAXLEN
index|]
decl_stmt|;
DECL|member|color_function
name|char
name|color_function
index|[
name|COLOR_MAXLEN
index|]
decl_stmt|;
DECL|member|color_lineno
name|char
name|color_lineno
index|[
name|COLOR_MAXLEN
index|]
decl_stmt|;
DECL|member|color_match
name|char
name|color_match
index|[
name|COLOR_MAXLEN
index|]
decl_stmt|;
DECL|member|color_selected
name|char
name|color_selected
index|[
name|COLOR_MAXLEN
index|]
decl_stmt|;
DECL|member|color_sep
name|char
name|color_sep
index|[
name|COLOR_MAXLEN
index|]
decl_stmt|;
DECL|member|regflags
name|int
name|regflags
decl_stmt|;
DECL|member|pre_context
name|unsigned
name|pre_context
decl_stmt|;
DECL|member|post_context
name|unsigned
name|post_context
decl_stmt|;
DECL|member|last_shown
name|unsigned
name|last_shown
decl_stmt|;
DECL|member|show_hunk_mark
name|int
name|show_hunk_mark
decl_stmt|;
DECL|member|file_break
name|int
name|file_break
decl_stmt|;
DECL|member|heading
name|int
name|heading
decl_stmt|;
DECL|member|priv
name|void
modifier|*
name|priv
decl_stmt|;
DECL|member|output
name|void
function_decl|(
modifier|*
name|output
function_decl|)
parameter_list|(
name|struct
name|grep_opt
modifier|*
name|opt
parameter_list|,
specifier|const
name|void
modifier|*
name|data
parameter_list|,
name|size_t
name|size
parameter_list|)
function_decl|;
DECL|member|output_priv
name|void
modifier|*
name|output_priv
decl_stmt|;
block|}
struct|;
end_struct
begin_function_decl
specifier|extern
name|void
name|init_grep_defaults
parameter_list|(
name|void
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|grep_config
parameter_list|(
specifier|const
name|char
modifier|*
name|var
parameter_list|,
specifier|const
name|char
modifier|*
name|value
parameter_list|,
name|void
modifier|*
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|void
name|grep_init
parameter_list|(
name|struct
name|grep_opt
modifier|*
parameter_list|,
specifier|const
name|char
modifier|*
name|prefix
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|void
name|grep_set_pattern_type_option
parameter_list|(
name|enum
name|grep_pattern_type
parameter_list|,
name|struct
name|grep_opt
modifier|*
name|opt
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|void
name|grep_commit_pattern_type
parameter_list|(
name|enum
name|grep_pattern_type
parameter_list|,
name|struct
name|grep_opt
modifier|*
name|opt
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|void
name|append_grep_pat
parameter_list|(
name|struct
name|grep_opt
modifier|*
name|opt
parameter_list|,
specifier|const
name|char
modifier|*
name|pat
parameter_list|,
name|size_t
name|patlen
parameter_list|,
specifier|const
name|char
modifier|*
name|origin
parameter_list|,
name|int
name|no
parameter_list|,
name|enum
name|grep_pat_token
name|t
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|void
name|append_grep_pattern
parameter_list|(
name|struct
name|grep_opt
modifier|*
name|opt
parameter_list|,
specifier|const
name|char
modifier|*
name|pat
parameter_list|,
specifier|const
name|char
modifier|*
name|origin
parameter_list|,
name|int
name|no
parameter_list|,
name|enum
name|grep_pat_token
name|t
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|void
name|append_header_grep_pattern
parameter_list|(
name|struct
name|grep_opt
modifier|*
parameter_list|,
name|enum
name|grep_header_field
parameter_list|,
specifier|const
name|char
modifier|*
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|void
name|compile_grep_patterns
parameter_list|(
name|struct
name|grep_opt
modifier|*
name|opt
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|void
name|free_grep_patterns
parameter_list|(
name|struct
name|grep_opt
modifier|*
name|opt
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|grep_buffer
parameter_list|(
name|struct
name|grep_opt
modifier|*
name|opt
parameter_list|,
name|char
modifier|*
name|buf
parameter_list|,
name|unsigned
name|long
name|size
parameter_list|)
function_decl|;
end_function_decl
begin_struct
DECL|struct|grep_source
struct|struct
name|grep_source
block|{
DECL|member|name
name|char
modifier|*
name|name
decl_stmt|;
DECL|enum|grep_source_type
enum|enum
name|grep_source_type
block|{
DECL|enumerator|GREP_SOURCE_SHA1
name|GREP_SOURCE_SHA1
block|,
DECL|enumerator|GREP_SOURCE_FILE
name|GREP_SOURCE_FILE
block|,
DECL|enumerator|GREP_SOURCE_BUF
name|GREP_SOURCE_BUF
block|, 	}
DECL|member|type
name|type
enum|;
DECL|member|identifier
name|void
modifier|*
name|identifier
decl_stmt|;
DECL|member|buf
name|char
modifier|*
name|buf
decl_stmt|;
DECL|member|size
name|unsigned
name|long
name|size
decl_stmt|;
DECL|member|path
name|char
modifier|*
name|path
decl_stmt|;
comment|/* for attribute lookups */
DECL|member|driver
name|struct
name|userdiff_driver
modifier|*
name|driver
decl_stmt|;
block|}
struct|;
end_struct
begin_function_decl
name|void
name|grep_source_init
parameter_list|(
name|struct
name|grep_source
modifier|*
name|gs
parameter_list|,
name|enum
name|grep_source_type
name|type
parameter_list|,
specifier|const
name|char
modifier|*
name|name
parameter_list|,
specifier|const
name|char
modifier|*
name|path
parameter_list|,
specifier|const
name|void
modifier|*
name|identifier
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|void
name|grep_source_clear_data
parameter_list|(
name|struct
name|grep_source
modifier|*
name|gs
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|void
name|grep_source_clear
parameter_list|(
name|struct
name|grep_source
modifier|*
name|gs
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|void
name|grep_source_load_driver
parameter_list|(
name|struct
name|grep_source
modifier|*
name|gs
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|int
name|grep_source
parameter_list|(
name|struct
name|grep_opt
modifier|*
name|opt
parameter_list|,
name|struct
name|grep_source
modifier|*
name|gs
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|struct
name|grep_opt
modifier|*
name|grep_opt_dup
parameter_list|(
specifier|const
name|struct
name|grep_opt
modifier|*
name|opt
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|grep_threads_ok
parameter_list|(
specifier|const
name|struct
name|grep_opt
modifier|*
name|opt
parameter_list|)
function_decl|;
end_function_decl
begin_ifndef
ifndef|#
directive|ifndef
name|NO_PTHREADS
end_ifndef
begin_comment
comment|/*  * Mutex used around access to the attributes machinery if  * opt->use_threads.  Must be initialized/destroyed by callers!  */
end_comment
begin_decl_stmt
specifier|extern
name|int
name|grep_use_locks
decl_stmt|;
end_decl_stmt
begin_decl_stmt
specifier|extern
name|pthread_mutex_t
name|grep_attr_mutex
decl_stmt|;
end_decl_stmt
begin_decl_stmt
specifier|extern
name|pthread_mutex_t
name|grep_read_mutex
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|grep_read_lock
specifier|static
specifier|inline
name|void
name|grep_read_lock
parameter_list|(
name|void
parameter_list|)
block|{
if|if
condition|(
name|grep_use_locks
condition|)
name|pthread_mutex_lock
argument_list|(
operator|&
name|grep_read_mutex
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|grep_read_unlock
specifier|static
specifier|inline
name|void
name|grep_read_unlock
parameter_list|(
name|void
parameter_list|)
block|{
if|if
condition|(
name|grep_use_locks
condition|)
name|pthread_mutex_unlock
argument_list|(
operator|&
name|grep_read_mutex
argument_list|)
expr_stmt|;
block|}
end_function
begin_else
else|#
directive|else
end_else
begin_define
DECL|macro|grep_read_lock
define|#
directive|define
name|grep_read_lock
parameter_list|()
end_define
begin_define
DECL|macro|grep_read_unlock
define|#
directive|define
name|grep_read_unlock
parameter_list|()
end_define
begin_endif
endif|#
directive|endif
end_endif
begin_endif
endif|#
directive|endif
end_endif
end_unit
