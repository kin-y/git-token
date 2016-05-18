begin_unit
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"string-list.h"
end_include
begin_function
DECL|function|string_list_init
name|void
name|string_list_init
parameter_list|(
name|struct
name|string_list
modifier|*
name|list
parameter_list|,
name|int
name|strdup_strings
parameter_list|)
block|{
name|memset
argument_list|(
name|list
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
operator|*
name|list
argument_list|)
argument_list|)
expr_stmt|;
name|list
operator|->
name|strdup_strings
operator|=
name|strdup_strings
expr_stmt|;
block|}
end_function
begin_comment
comment|/* if there is no exact match, point to the index where the entry could be  * inserted */
end_comment
begin_function
DECL|function|get_entry_index
specifier|static
name|int
name|get_entry_index
parameter_list|(
specifier|const
name|struct
name|string_list
modifier|*
name|list
parameter_list|,
specifier|const
name|char
modifier|*
name|string
parameter_list|,
name|int
modifier|*
name|exact_match
parameter_list|)
block|{
name|int
name|left
init|=
operator|-
literal|1
decl_stmt|,
name|right
init|=
name|list
operator|->
name|nr
decl_stmt|;
name|compare_strings_fn
name|cmp
init|=
name|list
operator|->
name|cmp
condition|?
name|list
operator|->
name|cmp
else|:
name|strcmp
decl_stmt|;
while|while
condition|(
name|left
operator|+
literal|1
operator|<
name|right
condition|)
block|{
name|int
name|middle
init|=
operator|(
name|left
operator|+
name|right
operator|)
operator|/
literal|2
decl_stmt|;
name|int
name|compare
init|=
name|cmp
argument_list|(
name|string
argument_list|,
name|list
operator|->
name|items
index|[
name|middle
index|]
operator|.
name|string
argument_list|)
decl_stmt|;
if|if
condition|(
name|compare
operator|<
literal|0
condition|)
name|right
operator|=
name|middle
expr_stmt|;
elseif|else
if|if
condition|(
name|compare
operator|>
literal|0
condition|)
name|left
operator|=
name|middle
expr_stmt|;
else|else
block|{
operator|*
name|exact_match
operator|=
literal|1
expr_stmt|;
return|return
name|middle
return|;
block|}
block|}
operator|*
name|exact_match
operator|=
literal|0
expr_stmt|;
return|return
name|right
return|;
block|}
end_function
begin_comment
comment|/* returns -1-index if already exists */
end_comment
begin_function
DECL|function|add_entry
specifier|static
name|int
name|add_entry
parameter_list|(
name|int
name|insert_at
parameter_list|,
name|struct
name|string_list
modifier|*
name|list
parameter_list|,
specifier|const
name|char
modifier|*
name|string
parameter_list|)
block|{
name|int
name|exact_match
init|=
literal|0
decl_stmt|;
name|int
name|index
init|=
name|insert_at
operator|!=
operator|-
literal|1
condition|?
name|insert_at
else|:
name|get_entry_index
argument_list|(
name|list
argument_list|,
name|string
argument_list|,
operator|&
name|exact_match
argument_list|)
decl_stmt|;
if|if
condition|(
name|exact_match
condition|)
return|return
operator|-
literal|1
operator|-
name|index
return|;
if|if
condition|(
name|list
operator|->
name|nr
operator|+
literal|1
operator|>=
name|list
operator|->
name|alloc
condition|)
block|{
name|list
operator|->
name|alloc
operator|+=
literal|32
expr_stmt|;
name|REALLOC_ARRAY
argument_list|(
name|list
operator|->
name|items
argument_list|,
name|list
operator|->
name|alloc
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|index
operator|<
name|list
operator|->
name|nr
condition|)
name|memmove
argument_list|(
name|list
operator|->
name|items
operator|+
name|index
operator|+
literal|1
argument_list|,
name|list
operator|->
name|items
operator|+
name|index
argument_list|,
operator|(
name|list
operator|->
name|nr
operator|-
name|index
operator|)
operator|*
sizeof|sizeof
argument_list|(
expr|struct
name|string_list_item
argument_list|)
argument_list|)
expr_stmt|;
name|list
operator|->
name|items
index|[
name|index
index|]
operator|.
name|string
operator|=
name|list
operator|->
name|strdup_strings
condition|?
name|xstrdup
argument_list|(
name|string
argument_list|)
else|:
operator|(
name|char
operator|*
operator|)
name|string
expr_stmt|;
name|list
operator|->
name|items
index|[
name|index
index|]
operator|.
name|util
operator|=
name|NULL
expr_stmt|;
name|list
operator|->
name|nr
operator|++
expr_stmt|;
return|return
name|index
return|;
block|}
end_function
begin_function
DECL|function|string_list_insert
name|struct
name|string_list_item
modifier|*
name|string_list_insert
parameter_list|(
name|struct
name|string_list
modifier|*
name|list
parameter_list|,
specifier|const
name|char
modifier|*
name|string
parameter_list|)
block|{
name|int
name|index
init|=
name|add_entry
argument_list|(
operator|-
literal|1
argument_list|,
name|list
argument_list|,
name|string
argument_list|)
decl_stmt|;
if|if
condition|(
name|index
operator|<
literal|0
condition|)
name|index
operator|=
operator|-
literal|1
operator|-
name|index
expr_stmt|;
return|return
name|list
operator|->
name|items
operator|+
name|index
return|;
block|}
end_function
begin_function
DECL|function|string_list_has_string
name|int
name|string_list_has_string
parameter_list|(
specifier|const
name|struct
name|string_list
modifier|*
name|list
parameter_list|,
specifier|const
name|char
modifier|*
name|string
parameter_list|)
block|{
name|int
name|exact_match
decl_stmt|;
name|get_entry_index
argument_list|(
name|list
argument_list|,
name|string
argument_list|,
operator|&
name|exact_match
argument_list|)
expr_stmt|;
return|return
name|exact_match
return|;
block|}
end_function
begin_function
DECL|function|string_list_find_insert_index
name|int
name|string_list_find_insert_index
parameter_list|(
specifier|const
name|struct
name|string_list
modifier|*
name|list
parameter_list|,
specifier|const
name|char
modifier|*
name|string
parameter_list|,
name|int
name|negative_existing_index
parameter_list|)
block|{
name|int
name|exact_match
decl_stmt|;
name|int
name|index
init|=
name|get_entry_index
argument_list|(
name|list
argument_list|,
name|string
argument_list|,
operator|&
name|exact_match
argument_list|)
decl_stmt|;
if|if
condition|(
name|exact_match
condition|)
name|index
operator|=
operator|-
literal|1
operator|-
operator|(
name|negative_existing_index
condition|?
name|index
else|:
literal|0
operator|)
expr_stmt|;
return|return
name|index
return|;
block|}
end_function
begin_function
DECL|function|string_list_lookup
name|struct
name|string_list_item
modifier|*
name|string_list_lookup
parameter_list|(
name|struct
name|string_list
modifier|*
name|list
parameter_list|,
specifier|const
name|char
modifier|*
name|string
parameter_list|)
block|{
name|int
name|exact_match
decl_stmt|,
name|i
init|=
name|get_entry_index
argument_list|(
name|list
argument_list|,
name|string
argument_list|,
operator|&
name|exact_match
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
name|exact_match
condition|)
return|return
name|NULL
return|;
return|return
name|list
operator|->
name|items
operator|+
name|i
return|;
block|}
end_function
begin_function
DECL|function|string_list_remove_duplicates
name|void
name|string_list_remove_duplicates
parameter_list|(
name|struct
name|string_list
modifier|*
name|list
parameter_list|,
name|int
name|free_util
parameter_list|)
block|{
if|if
condition|(
name|list
operator|->
name|nr
operator|>
literal|1
condition|)
block|{
name|int
name|src
decl_stmt|,
name|dst
decl_stmt|;
name|compare_strings_fn
name|cmp
init|=
name|list
operator|->
name|cmp
condition|?
name|list
operator|->
name|cmp
else|:
name|strcmp
decl_stmt|;
for|for
control|(
name|src
operator|=
name|dst
operator|=
literal|1
init|;
name|src
operator|<
name|list
operator|->
name|nr
condition|;
name|src
operator|++
control|)
block|{
if|if
condition|(
operator|!
name|cmp
argument_list|(
name|list
operator|->
name|items
index|[
name|dst
operator|-
literal|1
index|]
operator|.
name|string
argument_list|,
name|list
operator|->
name|items
index|[
name|src
index|]
operator|.
name|string
argument_list|)
condition|)
block|{
if|if
condition|(
name|list
operator|->
name|strdup_strings
condition|)
name|free
argument_list|(
name|list
operator|->
name|items
index|[
name|src
index|]
operator|.
name|string
argument_list|)
expr_stmt|;
if|if
condition|(
name|free_util
condition|)
name|free
argument_list|(
name|list
operator|->
name|items
index|[
name|src
index|]
operator|.
name|util
argument_list|)
expr_stmt|;
block|}
else|else
name|list
operator|->
name|items
index|[
name|dst
operator|++
index|]
operator|=
name|list
operator|->
name|items
index|[
name|src
index|]
expr_stmt|;
block|}
name|list
operator|->
name|nr
operator|=
name|dst
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|for_each_string_list
name|int
name|for_each_string_list
parameter_list|(
name|struct
name|string_list
modifier|*
name|list
parameter_list|,
name|string_list_each_func_t
name|fn
parameter_list|,
name|void
modifier|*
name|cb_data
parameter_list|)
block|{
name|int
name|i
decl_stmt|,
name|ret
init|=
literal|0
decl_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|list
operator|->
name|nr
condition|;
name|i
operator|++
control|)
if|if
condition|(
operator|(
name|ret
operator|=
name|fn
argument_list|(
operator|&
name|list
operator|->
name|items
index|[
name|i
index|]
argument_list|,
name|cb_data
argument_list|)
operator|)
condition|)
break|break;
return|return
name|ret
return|;
block|}
end_function
begin_function
DECL|function|filter_string_list
name|void
name|filter_string_list
parameter_list|(
name|struct
name|string_list
modifier|*
name|list
parameter_list|,
name|int
name|free_util
parameter_list|,
name|string_list_each_func_t
name|want
parameter_list|,
name|void
modifier|*
name|cb_data
parameter_list|)
block|{
name|int
name|src
decl_stmt|,
name|dst
init|=
literal|0
decl_stmt|;
for|for
control|(
name|src
operator|=
literal|0
init|;
name|src
operator|<
name|list
operator|->
name|nr
condition|;
name|src
operator|++
control|)
block|{
if|if
condition|(
name|want
argument_list|(
operator|&
name|list
operator|->
name|items
index|[
name|src
index|]
argument_list|,
name|cb_data
argument_list|)
condition|)
block|{
name|list
operator|->
name|items
index|[
name|dst
operator|++
index|]
operator|=
name|list
operator|->
name|items
index|[
name|src
index|]
expr_stmt|;
block|}
else|else
block|{
if|if
condition|(
name|list
operator|->
name|strdup_strings
condition|)
name|free
argument_list|(
name|list
operator|->
name|items
index|[
name|src
index|]
operator|.
name|string
argument_list|)
expr_stmt|;
if|if
condition|(
name|free_util
condition|)
name|free
argument_list|(
name|list
operator|->
name|items
index|[
name|src
index|]
operator|.
name|util
argument_list|)
expr_stmt|;
block|}
block|}
name|list
operator|->
name|nr
operator|=
name|dst
expr_stmt|;
block|}
end_function
begin_function
DECL|function|item_is_not_empty
specifier|static
name|int
name|item_is_not_empty
parameter_list|(
name|struct
name|string_list_item
modifier|*
name|item
parameter_list|,
name|void
modifier|*
name|unused
parameter_list|)
block|{
return|return
operator|*
name|item
operator|->
name|string
operator|!=
literal|'\0'
return|;
block|}
end_function
begin_function
DECL|function|string_list_remove_empty_items
name|void
name|string_list_remove_empty_items
parameter_list|(
name|struct
name|string_list
modifier|*
name|list
parameter_list|,
name|int
name|free_util
parameter_list|)
block|{
name|filter_string_list
argument_list|(
name|list
argument_list|,
name|free_util
argument_list|,
name|item_is_not_empty
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|string_list_clear
name|void
name|string_list_clear
parameter_list|(
name|struct
name|string_list
modifier|*
name|list
parameter_list|,
name|int
name|free_util
parameter_list|)
block|{
if|if
condition|(
name|list
operator|->
name|items
condition|)
block|{
name|int
name|i
decl_stmt|;
if|if
condition|(
name|list
operator|->
name|strdup_strings
condition|)
block|{
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|list
operator|->
name|nr
condition|;
name|i
operator|++
control|)
name|free
argument_list|(
name|list
operator|->
name|items
index|[
name|i
index|]
operator|.
name|string
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|free_util
condition|)
block|{
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|list
operator|->
name|nr
condition|;
name|i
operator|++
control|)
name|free
argument_list|(
name|list
operator|->
name|items
index|[
name|i
index|]
operator|.
name|util
argument_list|)
expr_stmt|;
block|}
name|free
argument_list|(
name|list
operator|->
name|items
argument_list|)
expr_stmt|;
block|}
name|list
operator|->
name|items
operator|=
name|NULL
expr_stmt|;
name|list
operator|->
name|nr
operator|=
name|list
operator|->
name|alloc
operator|=
literal|0
expr_stmt|;
block|}
end_function
begin_function
DECL|function|string_list_clear_func
name|void
name|string_list_clear_func
parameter_list|(
name|struct
name|string_list
modifier|*
name|list
parameter_list|,
name|string_list_clear_func_t
name|clearfunc
parameter_list|)
block|{
if|if
condition|(
name|list
operator|->
name|items
condition|)
block|{
name|int
name|i
decl_stmt|;
if|if
condition|(
name|clearfunc
condition|)
block|{
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|list
operator|->
name|nr
condition|;
name|i
operator|++
control|)
name|clearfunc
argument_list|(
name|list
operator|->
name|items
index|[
name|i
index|]
operator|.
name|util
argument_list|,
name|list
operator|->
name|items
index|[
name|i
index|]
operator|.
name|string
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|list
operator|->
name|strdup_strings
condition|)
block|{
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|list
operator|->
name|nr
condition|;
name|i
operator|++
control|)
name|free
argument_list|(
name|list
operator|->
name|items
index|[
name|i
index|]
operator|.
name|string
argument_list|)
expr_stmt|;
block|}
name|free
argument_list|(
name|list
operator|->
name|items
argument_list|)
expr_stmt|;
block|}
name|list
operator|->
name|items
operator|=
name|NULL
expr_stmt|;
name|list
operator|->
name|nr
operator|=
name|list
operator|->
name|alloc
operator|=
literal|0
expr_stmt|;
block|}
end_function
begin_function
DECL|function|print_string_list
name|void
name|print_string_list
parameter_list|(
specifier|const
name|struct
name|string_list
modifier|*
name|p
parameter_list|,
specifier|const
name|char
modifier|*
name|text
parameter_list|)
block|{
name|int
name|i
decl_stmt|;
if|if
condition|(
name|text
condition|)
name|printf
argument_list|(
literal|"%s\n"
argument_list|,
name|text
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
name|p
operator|->
name|nr
condition|;
name|i
operator|++
control|)
name|printf
argument_list|(
literal|"%s:%p\n"
argument_list|,
name|p
operator|->
name|items
index|[
name|i
index|]
operator|.
name|string
argument_list|,
name|p
operator|->
name|items
index|[
name|i
index|]
operator|.
name|util
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|string_list_append_nodup
name|struct
name|string_list_item
modifier|*
name|string_list_append_nodup
parameter_list|(
name|struct
name|string_list
modifier|*
name|list
parameter_list|,
name|char
modifier|*
name|string
parameter_list|)
block|{
name|struct
name|string_list_item
modifier|*
name|retval
decl_stmt|;
name|ALLOC_GROW
argument_list|(
name|list
operator|->
name|items
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
name|retval
operator|=
operator|&
name|list
operator|->
name|items
index|[
name|list
operator|->
name|nr
operator|++
index|]
expr_stmt|;
name|retval
operator|->
name|string
operator|=
name|string
expr_stmt|;
name|retval
operator|->
name|util
operator|=
name|NULL
expr_stmt|;
return|return
name|retval
return|;
block|}
end_function
begin_function
DECL|function|string_list_append
name|struct
name|string_list_item
modifier|*
name|string_list_append
parameter_list|(
name|struct
name|string_list
modifier|*
name|list
parameter_list|,
specifier|const
name|char
modifier|*
name|string
parameter_list|)
block|{
return|return
name|string_list_append_nodup
argument_list|(
name|list
argument_list|,
name|list
operator|->
name|strdup_strings
condition|?
name|xstrdup
argument_list|(
name|string
argument_list|)
else|:
operator|(
name|char
operator|*
operator|)
name|string
argument_list|)
return|;
block|}
end_function
begin_comment
comment|/* Yuck */
end_comment
begin_decl_stmt
DECL|variable|compare_for_qsort
specifier|static
name|compare_strings_fn
name|compare_for_qsort
decl_stmt|;
end_decl_stmt
begin_comment
comment|/* Only call this from inside string_list_sort! */
end_comment
begin_function
DECL|function|cmp_items
specifier|static
name|int
name|cmp_items
parameter_list|(
specifier|const
name|void
modifier|*
name|a
parameter_list|,
specifier|const
name|void
modifier|*
name|b
parameter_list|)
block|{
specifier|const
name|struct
name|string_list_item
modifier|*
name|one
init|=
name|a
decl_stmt|;
specifier|const
name|struct
name|string_list_item
modifier|*
name|two
init|=
name|b
decl_stmt|;
return|return
name|compare_for_qsort
argument_list|(
name|one
operator|->
name|string
argument_list|,
name|two
operator|->
name|string
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|string_list_sort
name|void
name|string_list_sort
parameter_list|(
name|struct
name|string_list
modifier|*
name|list
parameter_list|)
block|{
name|compare_for_qsort
operator|=
name|list
operator|->
name|cmp
condition|?
name|list
operator|->
name|cmp
else|:
name|strcmp
expr_stmt|;
name|qsort
argument_list|(
name|list
operator|->
name|items
argument_list|,
name|list
operator|->
name|nr
argument_list|,
sizeof|sizeof
argument_list|(
operator|*
name|list
operator|->
name|items
argument_list|)
argument_list|,
name|cmp_items
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|unsorted_string_list_lookup
name|struct
name|string_list_item
modifier|*
name|unsorted_string_list_lookup
parameter_list|(
name|struct
name|string_list
modifier|*
name|list
parameter_list|,
specifier|const
name|char
modifier|*
name|string
parameter_list|)
block|{
name|struct
name|string_list_item
modifier|*
name|item
decl_stmt|;
name|compare_strings_fn
name|cmp
init|=
name|list
operator|->
name|cmp
condition|?
name|list
operator|->
name|cmp
else|:
name|strcmp
decl_stmt|;
name|for_each_string_list_item
argument_list|(
argument|item
argument_list|,
argument|list
argument_list|)
if|if
condition|(
operator|!
name|cmp
argument_list|(
name|string
argument_list|,
name|item
operator|->
name|string
argument_list|)
condition|)
return|return
name|item
return|;
return|return
name|NULL
return|;
block|}
end_function
begin_function
DECL|function|unsorted_string_list_has_string
name|int
name|unsorted_string_list_has_string
parameter_list|(
name|struct
name|string_list
modifier|*
name|list
parameter_list|,
specifier|const
name|char
modifier|*
name|string
parameter_list|)
block|{
return|return
name|unsorted_string_list_lookup
argument_list|(
name|list
argument_list|,
name|string
argument_list|)
operator|!=
name|NULL
return|;
block|}
end_function
begin_function
DECL|function|unsorted_string_list_delete_item
name|void
name|unsorted_string_list_delete_item
parameter_list|(
name|struct
name|string_list
modifier|*
name|list
parameter_list|,
name|int
name|i
parameter_list|,
name|int
name|free_util
parameter_list|)
block|{
if|if
condition|(
name|list
operator|->
name|strdup_strings
condition|)
name|free
argument_list|(
name|list
operator|->
name|items
index|[
name|i
index|]
operator|.
name|string
argument_list|)
expr_stmt|;
if|if
condition|(
name|free_util
condition|)
name|free
argument_list|(
name|list
operator|->
name|items
index|[
name|i
index|]
operator|.
name|util
argument_list|)
expr_stmt|;
name|list
operator|->
name|items
index|[
name|i
index|]
operator|=
name|list
operator|->
name|items
index|[
name|list
operator|->
name|nr
operator|-
literal|1
index|]
expr_stmt|;
name|list
operator|->
name|nr
operator|--
expr_stmt|;
block|}
end_function
begin_function
DECL|function|string_list_split
name|int
name|string_list_split
parameter_list|(
name|struct
name|string_list
modifier|*
name|list
parameter_list|,
specifier|const
name|char
modifier|*
name|string
parameter_list|,
name|int
name|delim
parameter_list|,
name|int
name|maxsplit
parameter_list|)
block|{
name|int
name|count
init|=
literal|0
decl_stmt|;
specifier|const
name|char
modifier|*
name|p
init|=
name|string
decl_stmt|,
modifier|*
name|end
decl_stmt|;
if|if
condition|(
operator|!
name|list
operator|->
name|strdup_strings
condition|)
name|die
argument_list|(
literal|"internal error in string_list_split(): "
literal|"list->strdup_strings must be set"
argument_list|)
expr_stmt|;
for|for
control|(
init|;
condition|;
control|)
block|{
name|count
operator|++
expr_stmt|;
if|if
condition|(
name|maxsplit
operator|>=
literal|0
operator|&&
name|count
operator|>
name|maxsplit
condition|)
block|{
name|string_list_append
argument_list|(
name|list
argument_list|,
name|p
argument_list|)
expr_stmt|;
return|return
name|count
return|;
block|}
name|end
operator|=
name|strchr
argument_list|(
name|p
argument_list|,
name|delim
argument_list|)
expr_stmt|;
if|if
condition|(
name|end
condition|)
block|{
name|string_list_append_nodup
argument_list|(
name|list
argument_list|,
name|xmemdupz
argument_list|(
name|p
argument_list|,
name|end
operator|-
name|p
argument_list|)
argument_list|)
expr_stmt|;
name|p
operator|=
name|end
operator|+
literal|1
expr_stmt|;
block|}
else|else
block|{
name|string_list_append
argument_list|(
name|list
argument_list|,
name|p
argument_list|)
expr_stmt|;
return|return
name|count
return|;
block|}
block|}
block|}
end_function
begin_function
DECL|function|string_list_split_in_place
name|int
name|string_list_split_in_place
parameter_list|(
name|struct
name|string_list
modifier|*
name|list
parameter_list|,
name|char
modifier|*
name|string
parameter_list|,
name|int
name|delim
parameter_list|,
name|int
name|maxsplit
parameter_list|)
block|{
name|int
name|count
init|=
literal|0
decl_stmt|;
name|char
modifier|*
name|p
init|=
name|string
decl_stmt|,
modifier|*
name|end
decl_stmt|;
if|if
condition|(
name|list
operator|->
name|strdup_strings
condition|)
name|die
argument_list|(
literal|"internal error in string_list_split_in_place(): "
literal|"list->strdup_strings must not be set"
argument_list|)
expr_stmt|;
for|for
control|(
init|;
condition|;
control|)
block|{
name|count
operator|++
expr_stmt|;
if|if
condition|(
name|maxsplit
operator|>=
literal|0
operator|&&
name|count
operator|>
name|maxsplit
condition|)
block|{
name|string_list_append
argument_list|(
name|list
argument_list|,
name|p
argument_list|)
expr_stmt|;
return|return
name|count
return|;
block|}
name|end
operator|=
name|strchr
argument_list|(
name|p
argument_list|,
name|delim
argument_list|)
expr_stmt|;
if|if
condition|(
name|end
condition|)
block|{
operator|*
name|end
operator|=
literal|'\0'
expr_stmt|;
name|string_list_append
argument_list|(
name|list
argument_list|,
name|p
argument_list|)
expr_stmt|;
name|p
operator|=
name|end
operator|+
literal|1
expr_stmt|;
block|}
else|else
block|{
name|string_list_append
argument_list|(
name|list
argument_list|,
name|p
argument_list|)
expr_stmt|;
return|return
name|count
return|;
block|}
block|}
block|}
end_function
end_unit
