begin_unit
begin_include
include|#
directive|include
file|<stdio.h>
end_include
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"path-list.h"
end_include
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
name|path_list
modifier|*
name|list
parameter_list|,
specifier|const
name|char
modifier|*
name|path
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
name|strcmp
argument_list|(
name|path
argument_list|,
name|list
operator|->
name|items
index|[
name|middle
index|]
operator|.
name|path
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
name|struct
name|path_list
modifier|*
name|list
parameter_list|,
specifier|const
name|char
modifier|*
name|path
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
name|path
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
name|list
operator|->
name|items
operator|=
name|xrealloc
argument_list|(
name|list
operator|->
name|items
argument_list|,
name|list
operator|->
name|alloc
operator|*
sizeof|sizeof
argument_list|(
expr|struct
name|path_list_item
argument_list|)
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
name|path_list_item
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
name|path
operator|=
name|list
operator|->
name|strdup_paths
condition|?
name|xstrdup
argument_list|(
name|path
argument_list|)
else|:
operator|(
name|char
operator|*
operator|)
name|path
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
DECL|function|path_list_insert
name|struct
name|path_list_item
modifier|*
name|path_list_insert
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|,
name|struct
name|path_list
modifier|*
name|list
parameter_list|)
block|{
name|int
name|index
init|=
name|add_entry
argument_list|(
name|list
argument_list|,
name|path
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
DECL|function|path_list_has_path
name|int
name|path_list_has_path
parameter_list|(
specifier|const
name|struct
name|path_list
modifier|*
name|list
parameter_list|,
specifier|const
name|char
modifier|*
name|path
parameter_list|)
block|{
name|int
name|exact_match
decl_stmt|;
name|get_entry_index
argument_list|(
name|list
argument_list|,
name|path
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
DECL|function|path_list_lookup
name|struct
name|path_list_item
modifier|*
name|path_list_lookup
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|,
name|struct
name|path_list
modifier|*
name|list
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
name|path
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
DECL|function|path_list_clear
name|void
name|path_list_clear
parameter_list|(
name|struct
name|path_list
modifier|*
name|list
parameter_list|,
name|int
name|free_items
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
name|free_items
condition|)
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
block|{
if|if
condition|(
name|list
operator|->
name|strdup_paths
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
name|path
argument_list|)
expr_stmt|;
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
DECL|function|print_path_list
name|void
name|print_path_list
parameter_list|(
specifier|const
name|char
modifier|*
name|text
parameter_list|,
specifier|const
name|struct
name|path_list
modifier|*
name|p
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
name|path
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
end_unit
