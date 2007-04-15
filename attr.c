begin_unit
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"attr.h"
end_include
begin_comment
comment|/*  * The basic design decision here is that we are not going to have  * insanely large number of attributes.  *  * This is a randomly chosen prime.  */
end_comment
begin_define
DECL|macro|HASHSIZE
define|#
directive|define
name|HASHSIZE
value|257
end_define
begin_ifndef
ifndef|#
directive|ifndef
name|DEBUG_ATTR
end_ifndef
begin_define
DECL|macro|DEBUG_ATTR
define|#
directive|define
name|DEBUG_ATTR
value|0
end_define
begin_endif
endif|#
directive|endif
end_endif
begin_struct
DECL|struct|git_attr
struct|struct
name|git_attr
block|{
DECL|member|next
name|struct
name|git_attr
modifier|*
name|next
decl_stmt|;
DECL|member|h
name|unsigned
name|h
decl_stmt|;
DECL|member|attr_nr
name|int
name|attr_nr
decl_stmt|;
DECL|member|name
name|char
name|name
index|[
name|FLEX_ARRAY
index|]
decl_stmt|;
block|}
struct|;
end_struct
begin_decl_stmt
DECL|variable|attr_nr
specifier|static
name|int
name|attr_nr
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|check_all_attr
specifier|static
name|struct
name|git_attr_check
modifier|*
name|check_all_attr
decl_stmt|;
end_decl_stmt
begin_function_decl
DECL|variable|git_attr_hash
specifier|static
name|struct
name|git_attr
modifier|*
function_decl|(
name|git_attr_hash
function_decl|[
name|HASHSIZE
function_decl|]
end_function_decl
begin_empty_stmt
DECL|variable|git_attr_hash
unit|)
empty_stmt|;
end_empty_stmt
begin_function
DECL|function|hash_name
specifier|static
name|unsigned
name|hash_name
parameter_list|(
specifier|const
name|char
modifier|*
name|name
parameter_list|,
name|int
name|namelen
parameter_list|)
block|{
name|unsigned
name|val
init|=
literal|0
decl_stmt|;
name|unsigned
name|char
name|c
decl_stmt|;
while|while
condition|(
name|namelen
operator|--
condition|)
block|{
name|c
operator|=
operator|*
name|name
operator|++
expr_stmt|;
name|val
operator|=
operator|(
operator|(
name|val
operator|<<
literal|7
operator|)
operator||
operator|(
name|val
operator|>>
literal|22
operator|)
operator|)
operator|^
name|c
expr_stmt|;
block|}
return|return
name|val
return|;
block|}
end_function
begin_function
DECL|function|invalid_attr_name
specifier|static
name|int
name|invalid_attr_name
parameter_list|(
specifier|const
name|char
modifier|*
name|name
parameter_list|,
name|int
name|namelen
parameter_list|)
block|{
comment|/* 	 * Attribute name cannot begin with '-' and from 	 * [-A-Za-z0-9_.].  We'd specifically exclude '=' for now, 	 * as we might later want to allow non-binary value for 	 * attributes, e.g. "*.svg	merge=special-merge-program-for-svg" 	 */
if|if
condition|(
operator|*
name|name
operator|==
literal|'-'
condition|)
return|return
operator|-
literal|1
return|;
while|while
condition|(
name|namelen
operator|--
condition|)
block|{
name|char
name|ch
init|=
operator|*
name|name
operator|++
decl_stmt|;
if|if
condition|(
operator|!
operator|(
name|ch
operator|==
literal|'-'
operator|||
name|ch
operator|==
literal|'.'
operator|||
name|ch
operator|==
literal|'_'
operator|||
operator|(
literal|'0'
operator|<=
name|ch
operator|&&
name|ch
operator|<=
literal|'9'
operator|)
operator|||
operator|(
literal|'a'
operator|<=
name|ch
operator|&&
name|ch
operator|<=
literal|'z'
operator|)
operator|||
operator|(
literal|'A'
operator|<=
name|ch
operator|&&
name|ch
operator|<=
literal|'Z'
operator|)
operator|)
condition|)
return|return
operator|-
literal|1
return|;
block|}
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|git_attr
name|struct
name|git_attr
modifier|*
name|git_attr
parameter_list|(
specifier|const
name|char
modifier|*
name|name
parameter_list|,
name|int
name|len
parameter_list|)
block|{
name|unsigned
name|hval
init|=
name|hash_name
argument_list|(
name|name
argument_list|,
name|len
argument_list|)
decl_stmt|;
name|unsigned
name|pos
init|=
name|hval
operator|%
name|HASHSIZE
decl_stmt|;
name|struct
name|git_attr
modifier|*
name|a
decl_stmt|;
for|for
control|(
name|a
operator|=
name|git_attr_hash
index|[
name|pos
index|]
init|;
name|a
condition|;
name|a
operator|=
name|a
operator|->
name|next
control|)
block|{
if|if
condition|(
name|a
operator|->
name|h
operator|==
name|hval
operator|&&
operator|!
name|memcmp
argument_list|(
name|a
operator|->
name|name
argument_list|,
name|name
argument_list|,
name|len
argument_list|)
operator|&&
operator|!
name|a
operator|->
name|name
index|[
name|len
index|]
condition|)
return|return
name|a
return|;
block|}
if|if
condition|(
name|invalid_attr_name
argument_list|(
name|name
argument_list|,
name|len
argument_list|)
condition|)
return|return
name|NULL
return|;
name|a
operator|=
name|xmalloc
argument_list|(
sizeof|sizeof
argument_list|(
operator|*
name|a
argument_list|)
operator|+
name|len
operator|+
literal|1
argument_list|)
expr_stmt|;
name|memcpy
argument_list|(
name|a
operator|->
name|name
argument_list|,
name|name
argument_list|,
name|len
argument_list|)
expr_stmt|;
name|a
operator|->
name|name
index|[
name|len
index|]
operator|=
literal|0
expr_stmt|;
name|a
operator|->
name|h
operator|=
name|hval
expr_stmt|;
name|a
operator|->
name|next
operator|=
name|git_attr_hash
index|[
name|pos
index|]
expr_stmt|;
name|a
operator|->
name|attr_nr
operator|=
name|attr_nr
operator|++
expr_stmt|;
name|git_attr_hash
index|[
name|pos
index|]
operator|=
name|a
expr_stmt|;
name|check_all_attr
operator|=
name|xrealloc
argument_list|(
name|check_all_attr
argument_list|,
sizeof|sizeof
argument_list|(
operator|*
name|check_all_attr
argument_list|)
operator|*
name|attr_nr
argument_list|)
expr_stmt|;
name|check_all_attr
index|[
name|a
operator|->
name|attr_nr
index|]
operator|.
name|attr
operator|=
name|a
expr_stmt|;
return|return
name|a
return|;
block|}
end_function
begin_comment
comment|/*  * .gitattributes file is one line per record, each of which is  *  * (1) glob pattern.  * (2) whitespace  * (3) whitespace separated list of attribute names, each of which  *     could be prefixed with '-' to mean "not set".  */
end_comment
begin_struct
DECL|struct|attr_state
struct|struct
name|attr_state
block|{
DECL|member|unset
name|int
name|unset
decl_stmt|;
DECL|member|attr
name|struct
name|git_attr
modifier|*
name|attr
decl_stmt|;
block|}
struct|;
end_struct
begin_struct
DECL|struct|match_attr
struct|struct
name|match_attr
block|{
union|union
block|{
DECL|member|pattern
name|char
modifier|*
name|pattern
decl_stmt|;
DECL|member|attr
name|struct
name|git_attr
modifier|*
name|attr
decl_stmt|;
block|}
DECL|member|u
name|u
union|;
DECL|member|is_macro
name|char
name|is_macro
decl_stmt|;
DECL|member|num_attr
name|unsigned
name|num_attr
decl_stmt|;
DECL|member|state
name|struct
name|attr_state
name|state
index|[
name|FLEX_ARRAY
index|]
decl_stmt|;
block|}
struct|;
end_struct
begin_decl_stmt
DECL|variable|blank
specifier|static
specifier|const
name|char
name|blank
index|[]
init|=
literal|" \t\r\n"
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|parse_attr_line
specifier|static
name|struct
name|match_attr
modifier|*
name|parse_attr_line
parameter_list|(
specifier|const
name|char
modifier|*
name|line
parameter_list|,
specifier|const
name|char
modifier|*
name|src
parameter_list|,
name|int
name|lineno
parameter_list|,
name|int
name|macro_ok
parameter_list|)
block|{
name|int
name|namelen
decl_stmt|;
name|int
name|num_attr
decl_stmt|;
specifier|const
name|char
modifier|*
name|cp
decl_stmt|,
modifier|*
name|name
decl_stmt|;
name|struct
name|match_attr
modifier|*
name|res
init|=
name|res
decl_stmt|;
name|int
name|pass
decl_stmt|;
name|int
name|is_macro
decl_stmt|;
name|cp
operator|=
name|line
operator|+
name|strspn
argument_list|(
name|line
argument_list|,
name|blank
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
operator|*
name|cp
operator|||
operator|*
name|cp
operator|==
literal|'#'
condition|)
return|return
name|NULL
return|;
name|name
operator|=
name|cp
expr_stmt|;
name|namelen
operator|=
name|strcspn
argument_list|(
name|name
argument_list|,
name|blank
argument_list|)
expr_stmt|;
if|if
condition|(
name|strlen
argument_list|(
name|ATTRIBUTE_MACRO_PREFIX
argument_list|)
operator|<
name|namelen
operator|&&
operator|!
name|prefixcmp
argument_list|(
name|name
argument_list|,
name|ATTRIBUTE_MACRO_PREFIX
argument_list|)
condition|)
block|{
if|if
condition|(
operator|!
name|macro_ok
condition|)
block|{
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"%s not allowed: %s:%d\n"
argument_list|,
name|name
argument_list|,
name|src
argument_list|,
name|lineno
argument_list|)
expr_stmt|;
return|return
name|NULL
return|;
block|}
name|is_macro
operator|=
literal|1
expr_stmt|;
name|name
operator|+=
name|strlen
argument_list|(
name|ATTRIBUTE_MACRO_PREFIX
argument_list|)
expr_stmt|;
name|name
operator|+=
name|strspn
argument_list|(
name|name
argument_list|,
name|blank
argument_list|)
expr_stmt|;
name|namelen
operator|=
name|strcspn
argument_list|(
name|name
argument_list|,
name|blank
argument_list|)
expr_stmt|;
if|if
condition|(
name|invalid_attr_name
argument_list|(
name|name
argument_list|,
name|namelen
argument_list|)
condition|)
block|{
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"%.*s is not a valid attribute name: %s:%d\n"
argument_list|,
name|namelen
argument_list|,
name|name
argument_list|,
name|src
argument_list|,
name|lineno
argument_list|)
expr_stmt|;
return|return
name|NULL
return|;
block|}
block|}
else|else
name|is_macro
operator|=
literal|0
expr_stmt|;
for|for
control|(
name|pass
operator|=
literal|0
init|;
name|pass
operator|<
literal|2
condition|;
name|pass
operator|++
control|)
block|{
comment|/* pass 0 counts and allocates, pass 1 fills */
name|num_attr
operator|=
literal|0
expr_stmt|;
name|cp
operator|=
name|name
operator|+
name|namelen
expr_stmt|;
name|cp
operator|=
name|cp
operator|+
name|strspn
argument_list|(
name|cp
argument_list|,
name|blank
argument_list|)
expr_stmt|;
while|while
condition|(
operator|*
name|cp
condition|)
block|{
specifier|const
name|char
modifier|*
name|ep
decl_stmt|;
name|ep
operator|=
name|cp
operator|+
name|strcspn
argument_list|(
name|cp
argument_list|,
name|blank
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|pass
condition|)
block|{
if|if
condition|(
operator|*
name|cp
operator|==
literal|'-'
condition|)
name|cp
operator|++
expr_stmt|;
if|if
condition|(
name|invalid_attr_name
argument_list|(
name|cp
argument_list|,
name|ep
operator|-
name|cp
argument_list|)
condition|)
block|{
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"%.*s is not a valid attribute name: %s:%d\n"
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
argument_list|,
name|src
argument_list|,
name|lineno
argument_list|)
expr_stmt|;
return|return
name|NULL
return|;
block|}
block|}
else|else
block|{
name|struct
name|attr_state
modifier|*
name|e
decl_stmt|;
name|e
operator|=
operator|&
operator|(
name|res
operator|->
name|state
index|[
name|num_attr
index|]
operator|)
expr_stmt|;
if|if
condition|(
operator|*
name|cp
operator|==
literal|'-'
condition|)
block|{
name|e
operator|->
name|unset
operator|=
literal|1
expr_stmt|;
name|cp
operator|++
expr_stmt|;
block|}
name|e
operator|->
name|attr
operator|=
name|git_attr
argument_list|(
name|cp
argument_list|,
name|ep
operator|-
name|cp
argument_list|)
expr_stmt|;
block|}
name|num_attr
operator|++
expr_stmt|;
name|cp
operator|=
name|ep
operator|+
name|strspn
argument_list|(
name|ep
argument_list|,
name|blank
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|pass
condition|)
break|break;
name|res
operator|=
name|xcalloc
argument_list|(
literal|1
argument_list|,
sizeof|sizeof
argument_list|(
operator|*
name|res
argument_list|)
operator|+
sizeof|sizeof
argument_list|(
expr|struct
name|attr_state
argument_list|)
operator|*
name|num_attr
operator|+
operator|(
name|is_macro
condition|?
literal|0
else|:
name|namelen
operator|+
literal|1
operator|)
argument_list|)
expr_stmt|;
if|if
condition|(
name|is_macro
condition|)
block|{
name|res
operator|->
name|u
operator|.
name|attr
operator|=
name|git_attr
argument_list|(
name|name
argument_list|,
name|namelen
argument_list|)
expr_stmt|;
block|}
else|else
block|{
name|res
operator|->
name|u
operator|.
name|pattern
operator|=
operator|(
name|char
operator|*
operator|)
operator|&
operator|(
name|res
operator|->
name|state
index|[
name|num_attr
index|]
operator|)
expr_stmt|;
name|memcpy
argument_list|(
name|res
operator|->
name|u
operator|.
name|pattern
argument_list|,
name|name
argument_list|,
name|namelen
argument_list|)
expr_stmt|;
name|res
operator|->
name|u
operator|.
name|pattern
index|[
name|namelen
index|]
operator|=
literal|0
expr_stmt|;
block|}
name|res
operator|->
name|is_macro
operator|=
name|is_macro
expr_stmt|;
name|res
operator|->
name|num_attr
operator|=
name|num_attr
expr_stmt|;
block|}
return|return
name|res
return|;
block|}
end_function
begin_comment
comment|/*  * Like info/exclude and .gitignore, the attribute information can  * come from many places.  *  * (1) .gitattribute file of the same directory;  * (2) .gitattribute file of the parent directory if (1) does not have any match;  *     this goes recursively upwards, just like .gitignore  * (3) perhaps $GIT_DIR/info/attributes, as the final fallback.  *  * In the same file, later entries override the earlier match, so in the  * global list, we would have entries from info/attributes the earliest  * (reading the file from top to bottom), .gitattribute of the root  * directory (again, reading the file from top to bottom) down to the  * current directory, and then scan the list backwards to find the first match.  * This is exactly the same as what excluded() does in dir.c to deal with  * .gitignore  */
end_comment
begin_struct
DECL|struct|attr_stack
specifier|static
struct|struct
name|attr_stack
block|{
DECL|member|prev
name|struct
name|attr_stack
modifier|*
name|prev
decl_stmt|;
DECL|member|origin
name|char
modifier|*
name|origin
decl_stmt|;
DECL|member|num_matches
name|unsigned
name|num_matches
decl_stmt|;
DECL|member|attrs
name|struct
name|match_attr
modifier|*
modifier|*
name|attrs
decl_stmt|;
block|}
DECL|variable|attr_stack
modifier|*
name|attr_stack
struct|;
end_struct
begin_function
DECL|function|free_attr_elem
specifier|static
name|void
name|free_attr_elem
parameter_list|(
name|struct
name|attr_stack
modifier|*
name|e
parameter_list|)
block|{
name|int
name|i
decl_stmt|;
name|free
argument_list|(
name|e
operator|->
name|origin
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
name|e
operator|->
name|num_matches
condition|;
name|i
operator|++
control|)
name|free
argument_list|(
name|e
operator|->
name|attrs
index|[
name|i
index|]
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|e
argument_list|)
expr_stmt|;
block|}
end_function
begin_decl_stmt
DECL|variable|builtin_attr
specifier|static
specifier|const
name|char
modifier|*
name|builtin_attr
index|[]
init|=
block|{
literal|"[attr]binary -diff -crlf"
block|,
name|NULL
block|, }
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|read_attr_from_array
specifier|static
name|struct
name|attr_stack
modifier|*
name|read_attr_from_array
parameter_list|(
specifier|const
name|char
modifier|*
modifier|*
name|list
parameter_list|)
block|{
name|struct
name|attr_stack
modifier|*
name|res
decl_stmt|;
specifier|const
name|char
modifier|*
name|line
decl_stmt|;
name|int
name|lineno
init|=
literal|0
decl_stmt|;
name|res
operator|=
name|xcalloc
argument_list|(
literal|1
argument_list|,
sizeof|sizeof
argument_list|(
operator|*
name|res
argument_list|)
argument_list|)
expr_stmt|;
while|while
condition|(
operator|(
name|line
operator|=
operator|*
operator|(
name|list
operator|++
operator|)
operator|)
operator|!=
name|NULL
condition|)
block|{
name|struct
name|match_attr
modifier|*
name|a
decl_stmt|;
name|a
operator|=
name|parse_attr_line
argument_list|(
name|line
argument_list|,
literal|"[builtin]"
argument_list|,
operator|++
name|lineno
argument_list|,
literal|1
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|a
condition|)
continue|continue;
name|res
operator|->
name|attrs
operator|=
name|xrealloc
argument_list|(
name|res
operator|->
name|attrs
argument_list|,
name|res
operator|->
name|num_matches
operator|+
literal|1
argument_list|)
expr_stmt|;
name|res
operator|->
name|attrs
index|[
name|res
operator|->
name|num_matches
operator|++
index|]
operator|=
name|a
expr_stmt|;
block|}
return|return
name|res
return|;
block|}
end_function
begin_function
DECL|function|read_attr_from_file
specifier|static
name|struct
name|attr_stack
modifier|*
name|read_attr_from_file
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|,
name|int
name|macro_ok
parameter_list|)
block|{
name|FILE
modifier|*
name|fp
decl_stmt|;
name|struct
name|attr_stack
modifier|*
name|res
decl_stmt|;
name|char
name|buf
index|[
literal|2048
index|]
decl_stmt|;
name|int
name|lineno
init|=
literal|0
decl_stmt|;
name|res
operator|=
name|xcalloc
argument_list|(
literal|1
argument_list|,
sizeof|sizeof
argument_list|(
operator|*
name|res
argument_list|)
argument_list|)
expr_stmt|;
name|fp
operator|=
name|fopen
argument_list|(
name|path
argument_list|,
literal|"r"
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|fp
condition|)
return|return
name|res
return|;
while|while
condition|(
name|fgets
argument_list|(
name|buf
argument_list|,
sizeof|sizeof
argument_list|(
name|buf
argument_list|)
argument_list|,
name|fp
argument_list|)
condition|)
block|{
name|struct
name|match_attr
modifier|*
name|a
decl_stmt|;
name|a
operator|=
name|parse_attr_line
argument_list|(
name|buf
argument_list|,
name|path
argument_list|,
operator|++
name|lineno
argument_list|,
name|macro_ok
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|a
condition|)
continue|continue;
name|res
operator|->
name|attrs
operator|=
name|xrealloc
argument_list|(
name|res
operator|->
name|attrs
argument_list|,
name|res
operator|->
name|num_matches
operator|+
literal|1
argument_list|)
expr_stmt|;
name|res
operator|->
name|attrs
index|[
name|res
operator|->
name|num_matches
operator|++
index|]
operator|=
name|a
expr_stmt|;
block|}
name|fclose
argument_list|(
name|fp
argument_list|)
expr_stmt|;
return|return
name|res
return|;
block|}
end_function
begin_if
if|#
directive|if
name|DEBUG_ATTR
end_if
begin_function
DECL|function|debug_info
specifier|static
name|void
name|debug_info
parameter_list|(
specifier|const
name|char
modifier|*
name|what
parameter_list|,
name|struct
name|attr_stack
modifier|*
name|elem
parameter_list|)
block|{
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"%s: %s\n"
argument_list|,
name|what
argument_list|,
name|elem
operator|->
name|origin
condition|?
name|elem
operator|->
name|origin
else|:
literal|"()"
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|debug_set
specifier|static
name|void
name|debug_set
parameter_list|(
specifier|const
name|char
modifier|*
name|what
parameter_list|,
specifier|const
name|char
modifier|*
name|match
parameter_list|,
name|struct
name|git_attr
modifier|*
name|attr
parameter_list|,
name|int
name|set
parameter_list|)
block|{
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"%s: %s => %d (%s)\n"
argument_list|,
name|what
argument_list|,
name|attr
operator|->
name|name
argument_list|,
name|set
argument_list|,
name|match
argument_list|)
expr_stmt|;
block|}
end_function
begin_define
DECL|macro|debug_push
define|#
directive|define
name|debug_push
parameter_list|(
name|a
parameter_list|)
value|debug_info("push", (a))
end_define
begin_define
DECL|macro|debug_pop
define|#
directive|define
name|debug_pop
parameter_list|(
name|a
parameter_list|)
value|debug_info("pop", (a))
end_define
begin_else
else|#
directive|else
end_else
begin_define
DECL|macro|debug_push
define|#
directive|define
name|debug_push
parameter_list|(
name|a
parameter_list|)
value|do { ; } while (0)
end_define
begin_define
DECL|macro|debug_pop
define|#
directive|define
name|debug_pop
parameter_list|(
name|a
parameter_list|)
value|do { ; } while (0)
end_define
begin_define
DECL|macro|debug_set
define|#
directive|define
name|debug_set
parameter_list|(
name|a
parameter_list|,
name|b
parameter_list|,
name|c
parameter_list|,
name|d
parameter_list|)
value|do { ; } while (0)
end_define
begin_endif
endif|#
directive|endif
end_endif
begin_function
DECL|function|bootstrap_attr_stack
specifier|static
name|void
name|bootstrap_attr_stack
parameter_list|(
name|void
parameter_list|)
block|{
if|if
condition|(
operator|!
name|attr_stack
condition|)
block|{
name|struct
name|attr_stack
modifier|*
name|elem
decl_stmt|;
name|elem
operator|=
name|read_attr_from_array
argument_list|(
name|builtin_attr
argument_list|)
expr_stmt|;
name|elem
operator|->
name|origin
operator|=
name|NULL
expr_stmt|;
name|elem
operator|->
name|prev
operator|=
name|attr_stack
expr_stmt|;
name|attr_stack
operator|=
name|elem
expr_stmt|;
name|elem
operator|=
name|read_attr_from_file
argument_list|(
name|GITATTRIBUTES_FILE
argument_list|,
literal|1
argument_list|)
expr_stmt|;
name|elem
operator|->
name|origin
operator|=
name|strdup
argument_list|(
literal|""
argument_list|)
expr_stmt|;
name|elem
operator|->
name|prev
operator|=
name|attr_stack
expr_stmt|;
name|attr_stack
operator|=
name|elem
expr_stmt|;
name|debug_push
argument_list|(
name|elem
argument_list|)
expr_stmt|;
name|elem
operator|=
name|read_attr_from_file
argument_list|(
name|git_path
argument_list|(
name|INFOATTRIBUTES_FILE
argument_list|)
argument_list|,
literal|1
argument_list|)
expr_stmt|;
name|elem
operator|->
name|origin
operator|=
name|NULL
expr_stmt|;
name|elem
operator|->
name|prev
operator|=
name|attr_stack
expr_stmt|;
name|attr_stack
operator|=
name|elem
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|prepare_attr_stack
specifier|static
name|void
name|prepare_attr_stack
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|,
name|int
name|dirlen
parameter_list|)
block|{
name|struct
name|attr_stack
modifier|*
name|elem
decl_stmt|,
modifier|*
name|info
decl_stmt|;
name|int
name|len
decl_stmt|;
name|char
name|pathbuf
index|[
name|PATH_MAX
index|]
decl_stmt|;
comment|/* 	 * At the bottom of the attribute stack is the built-in 	 * set of attribute definitions.  Then, contents from 	 * .gitattribute files from directories closer to the 	 * root to the ones in deeper directories are pushed 	 * to the stack.  Finally, at the very top of the stack 	 * we always keep the contents of $GIT_DIR/info/attributes. 	 * 	 * When checking, we use entries from near the top of the 	 * stack, preferring $GIT_DIR/info/attributes, then 	 * .gitattributes in deeper directories to shallower ones, 	 * and finally use the built-in set as the default. 	 */
if|if
condition|(
operator|!
name|attr_stack
condition|)
name|bootstrap_attr_stack
argument_list|()
expr_stmt|;
comment|/* 	 * Pop the "info" one that is always at the top of the stack. 	 */
name|info
operator|=
name|attr_stack
expr_stmt|;
name|attr_stack
operator|=
name|info
operator|->
name|prev
expr_stmt|;
comment|/* 	 * Pop the ones from directories that are not the prefix of 	 * the path we are checking. 	 */
while|while
condition|(
name|attr_stack
operator|&&
name|attr_stack
operator|->
name|origin
condition|)
block|{
name|int
name|namelen
init|=
name|strlen
argument_list|(
name|attr_stack
operator|->
name|origin
argument_list|)
decl_stmt|;
name|elem
operator|=
name|attr_stack
expr_stmt|;
if|if
condition|(
name|namelen
operator|<=
name|dirlen
operator|&&
operator|!
name|strncmp
argument_list|(
name|elem
operator|->
name|origin
argument_list|,
name|path
argument_list|,
name|namelen
argument_list|)
condition|)
break|break;
name|debug_pop
argument_list|(
name|elem
argument_list|)
expr_stmt|;
name|attr_stack
operator|=
name|elem
operator|->
name|prev
expr_stmt|;
name|free_attr_elem
argument_list|(
name|elem
argument_list|)
expr_stmt|;
block|}
comment|/* 	 * Read from parent directories and push them down 	 */
while|while
condition|(
literal|1
condition|)
block|{
name|char
modifier|*
name|cp
decl_stmt|;
name|len
operator|=
name|strlen
argument_list|(
name|attr_stack
operator|->
name|origin
argument_list|)
expr_stmt|;
if|if
condition|(
name|dirlen
operator|<=
name|len
condition|)
break|break;
name|memcpy
argument_list|(
name|pathbuf
argument_list|,
name|path
argument_list|,
name|dirlen
argument_list|)
expr_stmt|;
name|memcpy
argument_list|(
name|pathbuf
operator|+
name|dirlen
argument_list|,
literal|"/"
argument_list|,
literal|2
argument_list|)
expr_stmt|;
name|cp
operator|=
name|strchr
argument_list|(
name|pathbuf
operator|+
name|len
operator|+
literal|1
argument_list|,
literal|'/'
argument_list|)
expr_stmt|;
name|strcpy
argument_list|(
name|cp
operator|+
literal|1
argument_list|,
name|GITATTRIBUTES_FILE
argument_list|)
expr_stmt|;
name|elem
operator|=
name|read_attr_from_file
argument_list|(
name|pathbuf
argument_list|,
literal|0
argument_list|)
expr_stmt|;
operator|*
name|cp
operator|=
literal|'\0'
expr_stmt|;
name|elem
operator|->
name|origin
operator|=
name|strdup
argument_list|(
name|pathbuf
argument_list|)
expr_stmt|;
name|elem
operator|->
name|prev
operator|=
name|attr_stack
expr_stmt|;
name|attr_stack
operator|=
name|elem
expr_stmt|;
name|debug_push
argument_list|(
name|elem
argument_list|)
expr_stmt|;
block|}
comment|/* 	 * Finally push the "info" one at the top of the stack. 	 */
name|info
operator|->
name|prev
operator|=
name|attr_stack
expr_stmt|;
name|attr_stack
operator|=
name|info
expr_stmt|;
block|}
end_function
begin_function
DECL|function|path_matches
specifier|static
name|int
name|path_matches
parameter_list|(
specifier|const
name|char
modifier|*
name|pathname
parameter_list|,
name|int
name|pathlen
parameter_list|,
specifier|const
name|char
modifier|*
name|pattern
parameter_list|,
specifier|const
name|char
modifier|*
name|base
parameter_list|,
name|int
name|baselen
parameter_list|)
block|{
if|if
condition|(
operator|!
name|strchr
argument_list|(
name|pattern
argument_list|,
literal|'/'
argument_list|)
condition|)
block|{
comment|/* match basename */
specifier|const
name|char
modifier|*
name|basename
init|=
name|strrchr
argument_list|(
name|pathname
argument_list|,
literal|'/'
argument_list|)
decl_stmt|;
name|basename
operator|=
name|basename
condition|?
name|basename
operator|+
literal|1
else|:
name|pathname
expr_stmt|;
return|return
operator|(
name|fnmatch
argument_list|(
name|pattern
argument_list|,
name|basename
argument_list|,
literal|0
argument_list|)
operator|==
literal|0
operator|)
return|;
block|}
comment|/* 	 * match with FNM_PATHNAME; the pattern has base implicitly 	 * in front of it. 	 */
if|if
condition|(
operator|*
name|pattern
operator|==
literal|'/'
condition|)
name|pattern
operator|++
expr_stmt|;
if|if
condition|(
name|pathlen
operator|<
name|baselen
operator|||
operator|(
name|baselen
operator|&&
name|pathname
index|[
name|baselen
operator|-
literal|1
index|]
operator|!=
literal|'/'
operator|)
operator|||
name|strncmp
argument_list|(
name|pathname
argument_list|,
name|base
argument_list|,
name|baselen
argument_list|)
condition|)
return|return
literal|0
return|;
return|return
name|fnmatch
argument_list|(
name|pattern
argument_list|,
name|pathname
operator|+
name|baselen
argument_list|,
name|FNM_PATHNAME
argument_list|)
operator|==
literal|0
return|;
block|}
end_function
begin_function
DECL|function|fill
specifier|static
name|int
name|fill
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|,
name|int
name|pathlen
parameter_list|,
name|struct
name|attr_stack
modifier|*
name|stk
parameter_list|,
name|int
name|rem
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|base
init|=
name|stk
operator|->
name|origin
condition|?
name|stk
operator|->
name|origin
else|:
literal|""
decl_stmt|;
name|int
name|i
decl_stmt|,
name|j
decl_stmt|;
name|struct
name|git_attr_check
modifier|*
name|check
init|=
name|check_all_attr
decl_stmt|;
for|for
control|(
name|i
operator|=
name|stk
operator|->
name|num_matches
operator|-
literal|1
init|;
literal|0
operator|<
name|rem
operator|&&
literal|0
operator|<=
name|i
condition|;
name|i
operator|--
control|)
block|{
name|struct
name|match_attr
modifier|*
name|a
init|=
name|stk
operator|->
name|attrs
index|[
name|i
index|]
decl_stmt|;
if|if
condition|(
name|a
operator|->
name|is_macro
condition|)
continue|continue;
if|if
condition|(
name|path_matches
argument_list|(
name|path
argument_list|,
name|pathlen
argument_list|,
name|a
operator|->
name|u
operator|.
name|pattern
argument_list|,
name|base
argument_list|,
name|strlen
argument_list|(
name|base
argument_list|)
argument_list|)
condition|)
block|{
for|for
control|(
name|j
operator|=
literal|0
init|;
literal|0
operator|<
name|rem
operator|&&
name|j
operator|<
name|a
operator|->
name|num_attr
condition|;
name|j
operator|++
control|)
block|{
name|struct
name|git_attr
modifier|*
name|attr
init|=
name|a
operator|->
name|state
index|[
name|j
index|]
operator|.
name|attr
decl_stmt|;
name|int
name|set
init|=
operator|!
name|a
operator|->
name|state
index|[
name|j
index|]
operator|.
name|unset
decl_stmt|;
name|int
modifier|*
name|n
init|=
operator|&
operator|(
name|check
index|[
name|attr
operator|->
name|attr_nr
index|]
operator|.
name|isset
operator|)
decl_stmt|;
if|if
condition|(
operator|*
name|n
operator|<
literal|0
condition|)
block|{
name|debug_set
argument_list|(
literal|"fill"
argument_list|,
name|a
operator|->
name|u
operator|.
name|pattern
argument_list|,
name|attr
argument_list|,
name|set
argument_list|)
expr_stmt|;
operator|*
name|n
operator|=
name|set
expr_stmt|;
name|rem
operator|--
expr_stmt|;
block|}
block|}
block|}
block|}
return|return
name|rem
return|;
block|}
end_function
begin_function
DECL|function|macroexpand
specifier|static
name|int
name|macroexpand
parameter_list|(
name|struct
name|attr_stack
modifier|*
name|stk
parameter_list|,
name|int
name|rem
parameter_list|)
block|{
name|int
name|i
decl_stmt|,
name|j
decl_stmt|;
name|struct
name|git_attr_check
modifier|*
name|check
init|=
name|check_all_attr
decl_stmt|;
for|for
control|(
name|i
operator|=
name|stk
operator|->
name|num_matches
operator|-
literal|1
init|;
literal|0
operator|<
name|rem
operator|&&
literal|0
operator|<=
name|i
condition|;
name|i
operator|--
control|)
block|{
name|struct
name|match_attr
modifier|*
name|a
init|=
name|stk
operator|->
name|attrs
index|[
name|i
index|]
decl_stmt|;
if|if
condition|(
operator|!
name|a
operator|->
name|is_macro
condition|)
continue|continue;
if|if
condition|(
name|check
index|[
name|a
operator|->
name|u
operator|.
name|attr
operator|->
name|attr_nr
index|]
operator|.
name|isset
operator|<
literal|0
condition|)
continue|continue;
for|for
control|(
name|j
operator|=
literal|0
init|;
literal|0
operator|<
name|rem
operator|&&
name|j
operator|<
name|a
operator|->
name|num_attr
condition|;
name|j
operator|++
control|)
block|{
name|struct
name|git_attr
modifier|*
name|attr
init|=
name|a
operator|->
name|state
index|[
name|j
index|]
operator|.
name|attr
decl_stmt|;
name|int
name|set
init|=
operator|!
name|a
operator|->
name|state
index|[
name|j
index|]
operator|.
name|unset
decl_stmt|;
name|int
modifier|*
name|n
init|=
operator|&
operator|(
name|check
index|[
name|attr
operator|->
name|attr_nr
index|]
operator|.
name|isset
operator|)
decl_stmt|;
if|if
condition|(
operator|*
name|n
operator|<
literal|0
condition|)
block|{
name|debug_set
argument_list|(
literal|"expand"
argument_list|,
name|a
operator|->
name|u
operator|.
name|attr
operator|->
name|name
argument_list|,
name|attr
argument_list|,
name|set
argument_list|)
expr_stmt|;
operator|*
name|n
operator|=
name|set
expr_stmt|;
name|rem
operator|--
expr_stmt|;
block|}
block|}
block|}
return|return
name|rem
return|;
block|}
end_function
begin_function
DECL|function|git_checkattr
name|int
name|git_checkattr
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|,
name|int
name|num
parameter_list|,
name|struct
name|git_attr_check
modifier|*
name|check
parameter_list|)
block|{
name|struct
name|attr_stack
modifier|*
name|stk
decl_stmt|;
specifier|const
name|char
modifier|*
name|cp
decl_stmt|;
name|int
name|dirlen
decl_stmt|,
name|pathlen
decl_stmt|,
name|i
decl_stmt|,
name|rem
decl_stmt|;
name|bootstrap_attr_stack
argument_list|()
expr_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|attr_nr
condition|;
name|i
operator|++
control|)
name|check_all_attr
index|[
name|i
index|]
operator|.
name|isset
operator|=
operator|-
literal|1
expr_stmt|;
name|pathlen
operator|=
name|strlen
argument_list|(
name|path
argument_list|)
expr_stmt|;
name|cp
operator|=
name|strrchr
argument_list|(
name|path
argument_list|,
literal|'/'
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|cp
condition|)
name|dirlen
operator|=
literal|0
expr_stmt|;
else|else
name|dirlen
operator|=
name|cp
operator|-
name|path
expr_stmt|;
name|prepare_attr_stack
argument_list|(
name|path
argument_list|,
name|dirlen
argument_list|)
expr_stmt|;
name|rem
operator|=
name|attr_nr
expr_stmt|;
for|for
control|(
name|stk
operator|=
name|attr_stack
init|;
literal|0
operator|<
name|rem
operator|&&
name|stk
condition|;
name|stk
operator|=
name|stk
operator|->
name|prev
control|)
name|rem
operator|=
name|fill
argument_list|(
name|path
argument_list|,
name|pathlen
argument_list|,
name|stk
argument_list|,
name|rem
argument_list|)
expr_stmt|;
for|for
control|(
name|stk
operator|=
name|attr_stack
init|;
literal|0
operator|<
name|rem
operator|&&
name|stk
condition|;
name|stk
operator|=
name|stk
operator|->
name|prev
control|)
name|rem
operator|=
name|macroexpand
argument_list|(
name|stk
argument_list|,
name|rem
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
name|num
condition|;
name|i
operator|++
control|)
name|check
index|[
name|i
index|]
operator|.
name|isset
operator|=
name|check_all_attr
index|[
name|check
index|[
name|i
index|]
operator|.
name|attr
operator|->
name|attr_nr
index|]
operator|.
name|isset
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
end_unit
