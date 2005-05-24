begin_unit
begin_comment
comment|/*  * Copyright (C) 2005 Junio C Hamano  */
end_comment
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"diff.h"
end_include
begin_include
include|#
directive|include
file|"diffcore.h"
end_include
begin_include
include|#
directive|include
file|"delta.h"
end_include
begin_struct
DECL|struct|diff_rename_pool
struct|struct
name|diff_rename_pool
block|{
DECL|member|s
name|struct
name|diff_filespec
modifier|*
modifier|*
name|s
decl_stmt|;
DECL|member|nr
DECL|member|alloc
name|int
name|nr
decl_stmt|,
name|alloc
decl_stmt|;
block|}
struct|;
end_struct
begin_function
DECL|function|diff_rename_pool_clear
specifier|static
name|void
name|diff_rename_pool_clear
parameter_list|(
name|struct
name|diff_rename_pool
modifier|*
name|pool
parameter_list|)
block|{
name|pool
operator|->
name|s
operator|=
name|NULL
expr_stmt|;
name|pool
operator|->
name|nr
operator|=
name|pool
operator|->
name|alloc
operator|=
literal|0
expr_stmt|;
block|}
end_function
begin_function
DECL|function|diff_rename_pool_add
specifier|static
name|void
name|diff_rename_pool_add
parameter_list|(
name|struct
name|diff_rename_pool
modifier|*
name|pool
parameter_list|,
name|struct
name|diff_filespec
modifier|*
name|s
parameter_list|)
block|{
if|if
condition|(
name|S_ISDIR
argument_list|(
name|s
operator|->
name|mode
argument_list|)
condition|)
return|return;
comment|/* no trees, please */
if|if
condition|(
name|pool
operator|->
name|alloc
operator|<=
name|pool
operator|->
name|nr
condition|)
block|{
name|pool
operator|->
name|alloc
operator|=
name|alloc_nr
argument_list|(
name|pool
operator|->
name|alloc
argument_list|)
expr_stmt|;
name|pool
operator|->
name|s
operator|=
name|xrealloc
argument_list|(
name|pool
operator|->
name|s
argument_list|,
sizeof|sizeof
argument_list|(
operator|*
operator|(
name|pool
operator|->
name|s
operator|)
argument_list|)
operator|*
name|pool
operator|->
name|alloc
argument_list|)
expr_stmt|;
block|}
name|pool
operator|->
name|s
index|[
name|pool
operator|->
name|nr
index|]
operator|=
name|s
expr_stmt|;
name|pool
operator|->
name|nr
operator|++
expr_stmt|;
block|}
end_function
begin_function
DECL|function|is_exact_match
specifier|static
name|int
name|is_exact_match
parameter_list|(
name|struct
name|diff_filespec
modifier|*
name|src
parameter_list|,
name|struct
name|diff_filespec
modifier|*
name|dst
parameter_list|)
block|{
if|if
condition|(
name|src
operator|->
name|sha1_valid
operator|&&
name|dst
operator|->
name|sha1_valid
operator|&&
operator|!
name|memcmp
argument_list|(
name|src
operator|->
name|sha1
argument_list|,
name|dst
operator|->
name|sha1
argument_list|,
literal|20
argument_list|)
condition|)
return|return
literal|1
return|;
if|if
condition|(
name|diff_populate_filespec
argument_list|(
name|src
argument_list|)
operator|||
name|diff_populate_filespec
argument_list|(
name|dst
argument_list|)
condition|)
comment|/* this is an error but will be caught downstream */
return|return
literal|0
return|;
if|if
condition|(
name|src
operator|->
name|size
operator|==
name|dst
operator|->
name|size
operator|&&
operator|!
name|memcmp
argument_list|(
name|src
operator|->
name|data
argument_list|,
name|dst
operator|->
name|data
argument_list|,
name|src
operator|->
name|size
argument_list|)
condition|)
return|return
literal|1
return|;
return|return
literal|0
return|;
block|}
end_function
begin_struct
DECL|struct|diff_score
struct|struct
name|diff_score
block|{
DECL|member|src
name|struct
name|diff_filespec
modifier|*
name|src
decl_stmt|;
DECL|member|dst
name|struct
name|diff_filespec
modifier|*
name|dst
decl_stmt|;
DECL|member|score
name|int
name|score
decl_stmt|;
DECL|member|rank
name|int
name|rank
decl_stmt|;
block|}
struct|;
end_struct
begin_function
DECL|function|estimate_similarity
specifier|static
name|int
name|estimate_similarity
parameter_list|(
name|struct
name|diff_filespec
modifier|*
name|src
parameter_list|,
name|struct
name|diff_filespec
modifier|*
name|dst
parameter_list|,
name|int
name|minimum_score
parameter_list|)
block|{
comment|/* src points at a file that existed in the original tree (or 	 * optionally a file in the destination tree) and dst points 	 * at a newly created file.  They may be quite similar, in which 	 * case we want to say src is renamed to dst or src is copied into 	 * dst, and then some edit has been applied to dst. 	 * 	 * Compare them and return how similar they are, representing 	 * the score as an integer between 0 and 10000, except 	 * where they match exactly it is considered better than anything 	 * else. 	 */
name|void
modifier|*
name|delta
decl_stmt|;
name|unsigned
name|long
name|delta_size
decl_stmt|,
name|base_size
decl_stmt|;
name|int
name|score
decl_stmt|;
comment|/* We deal only with regular files.  Symlink renames are handled 	 * only when they are exact matches --- in other words, no edits 	 * after renaming. 	 */
if|if
condition|(
operator|!
name|S_ISREG
argument_list|(
name|src
operator|->
name|mode
argument_list|)
operator|||
operator|!
name|S_ISREG
argument_list|(
name|dst
operator|->
name|mode
argument_list|)
condition|)
return|return
literal|0
return|;
name|delta_size
operator|=
operator|(
operator|(
name|src
operator|->
name|size
operator|<
name|dst
operator|->
name|size
operator|)
condition|?
operator|(
name|dst
operator|->
name|size
operator|-
name|src
operator|->
name|size
operator|)
else|:
operator|(
name|src
operator|->
name|size
operator|-
name|dst
operator|->
name|size
operator|)
operator|)
expr_stmt|;
name|base_size
operator|=
operator|(
operator|(
name|src
operator|->
name|size
operator|<
name|dst
operator|->
name|size
operator|)
condition|?
name|src
operator|->
name|size
else|:
name|dst
operator|->
name|size
operator|)
expr_stmt|;
comment|/* We would not consider edits that change the file size so 	 * drastically.  delta_size must be smaller than 	 * (MAX_SCORE-minimum_score)/MAX_SCORE * min(src->size, dst->size). 	 * Note that base_size == 0 case is handled here already 	 * and the final score computation below would not have a 	 * divide-by-zero issue. 	 */
if|if
condition|(
name|base_size
operator|*
operator|(
name|MAX_SCORE
operator|-
name|minimum_score
operator|)
operator|<
name|delta_size
operator|*
name|MAX_SCORE
condition|)
return|return
literal|0
return|;
name|delta
operator|=
name|diff_delta
argument_list|(
name|src
operator|->
name|data
argument_list|,
name|src
operator|->
name|size
argument_list|,
name|dst
operator|->
name|data
argument_list|,
name|dst
operator|->
name|size
argument_list|,
operator|&
name|delta_size
argument_list|)
expr_stmt|;
comment|/* 	 * We currently punt here, but we may later end up parsing the 	 * delta to really assess the extent of damage.  A big consecutive 	 * remove would produce small delta_size that affects quite a 	 * big portion of the file. 	 */
name|free
argument_list|(
name|delta
argument_list|)
expr_stmt|;
comment|/* 	 * Now we will give some score to it.  100% edit gets 0 points 	 * and 0% edit gets MAX_SCORE points. 	 */
name|score
operator|=
name|MAX_SCORE
operator|-
operator|(
name|MAX_SCORE
operator|*
name|delta_size
operator|/
name|base_size
operator|)
expr_stmt|;
if|if
condition|(
name|score
operator|<
literal|0
condition|)
return|return
literal|0
return|;
if|if
condition|(
name|MAX_SCORE
operator|<
name|score
condition|)
return|return
name|MAX_SCORE
return|;
return|return
name|score
return|;
block|}
end_function
begin_function
DECL|function|record_rename_pair
specifier|static
name|void
name|record_rename_pair
parameter_list|(
name|struct
name|diff_queue_struct
modifier|*
name|outq
parameter_list|,
name|struct
name|diff_filespec
modifier|*
name|src
parameter_list|,
name|struct
name|diff_filespec
modifier|*
name|dst
parameter_list|,
name|int
name|rank
parameter_list|,
name|int
name|score
parameter_list|)
block|{
comment|/* 	 * These ranks are used to sort the final output, because there 	 * are certain dependencies: 	 * 	 *  1. rename/copy that depends on deleted ones. 	 *  2. deletions in the original. 	 *  3. rename/copy that depends on the pre-edit image of kept files. 	 *  4. additions, modifications and no-modifications in the original. 	 *  5. rename/copy that depends on the post-edit image of kept files 	 *     (note that we currently do not detect such rename/copy). 	 * 	 * The downstream diffcore transformers are free to reorder 	 * the entries as long as they keep file pairs that has the 	 * same p->one->path in earlier rename_rank to appear before 	 * later ones. 	 * 	 * To the final output routine, and in the diff-raw format 	 * output, a rename/copy that is based on a path that has a 	 * later entry that shares the same p->one->path and is not a 	 * deletion is a copy.  Otherwise it is a rename. 	 */
name|struct
name|diff_filepair
modifier|*
name|dp
init|=
name|diff_queue
argument_list|(
name|outq
argument_list|,
name|src
argument_list|,
name|dst
argument_list|)
decl_stmt|;
name|dp
operator|->
name|rename_rank
operator|=
name|rank
operator|*
literal|2
operator|+
literal|1
expr_stmt|;
name|dp
operator|->
name|score
operator|=
name|score
expr_stmt|;
name|dst
operator|->
name|xfrm_flags
operator||=
name|RENAME_DST_MATCHED
expr_stmt|;
block|}
end_function
begin_if
if|#
directive|if
literal|0
end_if
begin_else
unit|static void debug_filespec(struct diff_filespec *s, int x, const char *one) { 	fprintf(stderr, "queue[%d] %s (%s) %s %06o %s\n", 		x, one, 		s->path, 		DIFF_FILE_VALID(s) ? "valid" : "invalid", 		s->mode, 		s->sha1_valid ? sha1_to_hex(s->sha1) : ""); 	fprintf(stderr, "queue[%d] %s size %lu flags %d\n", 		x, one, 		s->size, s->xfrm_flags); }  static void debug_filepair(const struct diff_filepair *p, int i) { 	debug_filespec(p->one, i, "one"); 	debug_filespec(p->two, i, "two"); 	fprintf(stderr, "pair rank %d, orig order %d, score %d\n", 		p->rename_rank, p->orig_order, p->score); }  static void debug_queue(const char *msg, struct diff_queue_struct *q) { 	int i; 	if (msg) 		fprintf(stderr, "%s\n", msg); 	fprintf(stderr, "q->nr = %d\n", q->nr); 	for (i = 0; i< q->nr; i++) { 		struct diff_filepair *p = q->queue[i]; 		debug_filepair(p, i); 	} }
else|#
directive|else
end_else
begin_define
DECL|macro|debug_queue
define|#
directive|define
name|debug_queue
parameter_list|(
name|a
parameter_list|,
name|b
parameter_list|)
value|do { ;
comment|/*nothing*/
value|} while(0)
end_define
begin_endif
endif|#
directive|endif
end_endif
begin_comment
comment|/*  * We sort the outstanding diff entries according to the rank (see  * comment at the beginning of record_rename_pair) and tiebreak with  * the order in the original input.  */
end_comment
begin_function
DECL|function|rank_compare
specifier|static
name|int
name|rank_compare
parameter_list|(
specifier|const
name|void
modifier|*
name|a_
parameter_list|,
specifier|const
name|void
modifier|*
name|b_
parameter_list|)
block|{
specifier|const
name|struct
name|diff_filepair
modifier|*
name|a
init|=
operator|*
operator|(
specifier|const
expr|struct
name|diff_filepair
operator|*
operator|*
operator|)
name|a_
decl_stmt|;
specifier|const
name|struct
name|diff_filepair
modifier|*
name|b
init|=
operator|*
operator|(
specifier|const
expr|struct
name|diff_filepair
operator|*
operator|*
operator|)
name|b_
decl_stmt|;
name|int
name|a_rank
init|=
name|a
operator|->
name|rename_rank
decl_stmt|;
name|int
name|b_rank
init|=
name|b
operator|->
name|rename_rank
decl_stmt|;
if|if
condition|(
name|a_rank
operator|!=
name|b_rank
condition|)
return|return
name|a_rank
operator|-
name|b_rank
return|;
return|return
name|a
operator|->
name|orig_order
operator|-
name|b
operator|->
name|orig_order
return|;
block|}
end_function
begin_comment
comment|/*  * We sort the rename similarity matrix with the score, in descending  * order (more similar first).  */
end_comment
begin_function
DECL|function|score_compare
specifier|static
name|int
name|score_compare
parameter_list|(
specifier|const
name|void
modifier|*
name|a_
parameter_list|,
specifier|const
name|void
modifier|*
name|b_
parameter_list|)
block|{
specifier|const
name|struct
name|diff_score
modifier|*
name|a
init|=
name|a_
decl_stmt|,
modifier|*
name|b
init|=
name|b_
decl_stmt|;
return|return
name|b
operator|->
name|score
operator|-
name|a
operator|->
name|score
return|;
block|}
end_function
begin_function
DECL|function|diff_scoreopt_parse
name|int
name|diff_scoreopt_parse
parameter_list|(
specifier|const
name|char
modifier|*
name|opt
parameter_list|)
block|{
name|int
name|diglen
decl_stmt|,
name|num
decl_stmt|,
name|scale
decl_stmt|,
name|i
decl_stmt|;
if|if
condition|(
name|opt
index|[
literal|0
index|]
operator|!=
literal|'-'
operator|||
operator|(
name|opt
index|[
literal|1
index|]
operator|!=
literal|'M'
operator|&&
name|opt
index|[
literal|1
index|]
operator|!=
literal|'C'
operator|)
condition|)
return|return
operator|-
literal|1
return|;
comment|/* that is not a -M nor -C option */
name|diglen
operator|=
name|strspn
argument_list|(
name|opt
operator|+
literal|2
argument_list|,
literal|"0123456789"
argument_list|)
expr_stmt|;
if|if
condition|(
name|diglen
operator|==
literal|0
operator|||
name|strlen
argument_list|(
name|opt
operator|+
literal|2
argument_list|)
operator|!=
name|diglen
condition|)
return|return
literal|0
return|;
comment|/* use default */
name|sscanf
argument_list|(
name|opt
operator|+
literal|2
argument_list|,
literal|"%d"
argument_list|,
operator|&
name|num
argument_list|)
expr_stmt|;
for|for
control|(
name|i
operator|=
literal|0
operator|,
name|scale
operator|=
literal|1
init|;
name|i
operator|<
name|diglen
condition|;
name|i
operator|++
control|)
name|scale
operator|*=
literal|10
expr_stmt|;
comment|/* user says num divided by scale and we say internally that 	 * is MAX_SCORE * num / scale. 	 */
return|return
name|MAX_SCORE
operator|*
name|num
operator|/
name|scale
return|;
block|}
end_function
begin_function
DECL|function|diffcore_rename
name|void
name|diffcore_rename
parameter_list|(
name|int
name|detect_rename
parameter_list|,
name|int
name|minimum_score
parameter_list|)
block|{
name|struct
name|diff_queue_struct
modifier|*
name|q
init|=
operator|&
name|diff_queued_diff
decl_stmt|;
name|struct
name|diff_queue_struct
name|outq
decl_stmt|;
name|struct
name|diff_rename_pool
name|created
decl_stmt|,
name|deleted
decl_stmt|,
name|stay
decl_stmt|;
name|struct
name|diff_rename_pool
modifier|*
argument_list|(
name|srcs
index|[
literal|2
index|]
argument_list|)
decl_stmt|;
name|struct
name|diff_score
modifier|*
name|mx
decl_stmt|;
name|int
name|h
decl_stmt|,
name|i
decl_stmt|,
name|j
decl_stmt|;
name|int
name|num_create
decl_stmt|,
name|num_src
decl_stmt|,
name|dst_cnt
decl_stmt|,
name|src_cnt
decl_stmt|;
if|if
condition|(
operator|!
name|minimum_score
condition|)
name|minimum_score
operator|=
name|DEFAULT_MINIMUM_SCORE
expr_stmt|;
name|outq
operator|.
name|queue
operator|=
name|NULL
expr_stmt|;
name|outq
operator|.
name|nr
operator|=
name|outq
operator|.
name|alloc
operator|=
literal|0
expr_stmt|;
name|diff_rename_pool_clear
argument_list|(
operator|&
name|created
argument_list|)
expr_stmt|;
name|diff_rename_pool_clear
argument_list|(
operator|&
name|deleted
argument_list|)
expr_stmt|;
name|diff_rename_pool_clear
argument_list|(
operator|&
name|stay
argument_list|)
expr_stmt|;
name|srcs
index|[
literal|0
index|]
operator|=
operator|&
name|deleted
expr_stmt|;
name|srcs
index|[
literal|1
index|]
operator|=
operator|&
name|stay
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
block|{
name|struct
name|diff_filepair
modifier|*
name|p
init|=
name|q
operator|->
name|queue
index|[
name|i
index|]
decl_stmt|;
if|if
condition|(
operator|!
name|DIFF_FILE_VALID
argument_list|(
name|p
operator|->
name|one
argument_list|)
condition|)
if|if
condition|(
operator|!
name|DIFF_FILE_VALID
argument_list|(
name|p
operator|->
name|two
argument_list|)
condition|)
continue|continue;
comment|/* unmerged */
else|else
name|diff_rename_pool_add
argument_list|(
operator|&
name|created
argument_list|,
name|p
operator|->
name|two
argument_list|)
expr_stmt|;
elseif|else
if|if
condition|(
operator|!
name|DIFF_FILE_VALID
argument_list|(
name|p
operator|->
name|two
argument_list|)
condition|)
name|diff_rename_pool_add
argument_list|(
operator|&
name|deleted
argument_list|,
name|p
operator|->
name|one
argument_list|)
expr_stmt|;
elseif|else
if|if
condition|(
literal|1
operator|<
name|detect_rename
condition|)
comment|/* find copy, too */
name|diff_rename_pool_add
argument_list|(
operator|&
name|stay
argument_list|,
name|p
operator|->
name|one
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|created
operator|.
name|nr
operator|==
literal|0
condition|)
goto|goto
name|cleanup
goto|;
comment|/* nothing to do */
comment|/* We really want to cull the candidates list early 	 * with cheap tests in order to avoid doing deltas. 	 */
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|created
operator|.
name|nr
condition|;
name|i
operator|++
control|)
block|{
for|for
control|(
name|h
operator|=
literal|0
init|;
name|h
operator|<
sizeof|sizeof
argument_list|(
name|srcs
argument_list|)
operator|/
sizeof|sizeof
argument_list|(
name|srcs
index|[
literal|0
index|]
argument_list|)
condition|;
name|h
operator|++
control|)
block|{
name|struct
name|diff_rename_pool
modifier|*
name|p
init|=
name|srcs
index|[
name|h
index|]
decl_stmt|;
for|for
control|(
name|j
operator|=
literal|0
init|;
name|j
operator|<
name|p
operator|->
name|nr
condition|;
name|j
operator|++
control|)
block|{
if|if
condition|(
operator|!
name|is_exact_match
argument_list|(
name|p
operator|->
name|s
index|[
name|j
index|]
argument_list|,
name|created
operator|.
name|s
index|[
name|i
index|]
argument_list|)
condition|)
continue|continue;
name|record_rename_pair
argument_list|(
operator|&
name|outq
argument_list|,
name|p
operator|->
name|s
index|[
name|j
index|]
argument_list|,
name|created
operator|.
name|s
index|[
name|i
index|]
argument_list|,
name|h
argument_list|,
name|MAX_SCORE
argument_list|)
expr_stmt|;
break|break;
comment|/* we are done with this entry */
block|}
block|}
block|}
name|debug_queue
argument_list|(
literal|"done detecting exact"
argument_list|,
operator|&
name|outq
argument_list|)
expr_stmt|;
comment|/* Have we run out the created file pool?  If so we can avoid 	 * doing the delta matrix altogether. 	 */
if|if
condition|(
name|outq
operator|.
name|nr
operator|==
name|created
operator|.
name|nr
condition|)
goto|goto
name|flush_rest
goto|;
name|num_create
operator|=
operator|(
name|created
operator|.
name|nr
operator|-
name|outq
operator|.
name|nr
operator|)
expr_stmt|;
name|num_src
operator|=
name|deleted
operator|.
name|nr
operator|+
name|stay
operator|.
name|nr
expr_stmt|;
name|mx
operator|=
name|xmalloc
argument_list|(
sizeof|sizeof
argument_list|(
operator|*
name|mx
argument_list|)
operator|*
name|num_create
operator|*
name|num_src
argument_list|)
expr_stmt|;
for|for
control|(
name|dst_cnt
operator|=
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|created
operator|.
name|nr
condition|;
name|i
operator|++
control|)
block|{
name|int
name|base
init|=
name|dst_cnt
operator|*
name|num_src
decl_stmt|;
if|if
condition|(
name|created
operator|.
name|s
index|[
name|i
index|]
operator|->
name|xfrm_flags
operator|&
name|RENAME_DST_MATCHED
condition|)
continue|continue;
comment|/* dealt with exact match already. */
for|for
control|(
name|src_cnt
operator|=
name|h
operator|=
literal|0
init|;
name|h
operator|<
sizeof|sizeof
argument_list|(
name|srcs
argument_list|)
operator|/
sizeof|sizeof
argument_list|(
name|srcs
index|[
literal|0
index|]
argument_list|)
condition|;
name|h
operator|++
control|)
block|{
name|struct
name|diff_rename_pool
modifier|*
name|p
init|=
name|srcs
index|[
name|h
index|]
decl_stmt|;
for|for
control|(
name|j
operator|=
literal|0
init|;
name|j
operator|<
name|p
operator|->
name|nr
condition|;
name|j
operator|++
operator|,
name|src_cnt
operator|++
control|)
block|{
name|struct
name|diff_score
modifier|*
name|m
init|=
operator|&
name|mx
index|[
name|base
operator|+
name|src_cnt
index|]
decl_stmt|;
name|m
operator|->
name|src
operator|=
name|p
operator|->
name|s
index|[
name|j
index|]
expr_stmt|;
name|m
operator|->
name|dst
operator|=
name|created
operator|.
name|s
index|[
name|i
index|]
expr_stmt|;
name|m
operator|->
name|score
operator|=
name|estimate_similarity
argument_list|(
name|m
operator|->
name|src
argument_list|,
name|m
operator|->
name|dst
argument_list|,
name|minimum_score
argument_list|)
expr_stmt|;
name|m
operator|->
name|rank
operator|=
name|h
expr_stmt|;
block|}
block|}
name|dst_cnt
operator|++
expr_stmt|;
block|}
comment|/* cost matrix sorted by most to least similar pair */
name|qsort
argument_list|(
name|mx
argument_list|,
name|num_create
operator|*
name|num_src
argument_list|,
sizeof|sizeof
argument_list|(
operator|*
name|mx
argument_list|)
argument_list|,
name|score_compare
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
name|num_create
operator|*
name|num_src
condition|;
name|i
operator|++
control|)
block|{
if|if
condition|(
name|mx
index|[
name|i
index|]
operator|.
name|dst
operator|->
name|xfrm_flags
operator|&
name|RENAME_DST_MATCHED
condition|)
continue|continue;
comment|/* alreayd done, either exact or fuzzy. */
if|if
condition|(
name|mx
index|[
name|i
index|]
operator|.
name|score
operator|<
name|minimum_score
condition|)
break|break;
comment|/* there is not any more diffs applicable. */
name|record_rename_pair
argument_list|(
operator|&
name|outq
argument_list|,
name|mx
index|[
name|i
index|]
operator|.
name|src
argument_list|,
name|mx
index|[
name|i
index|]
operator|.
name|dst
argument_list|,
name|mx
index|[
name|i
index|]
operator|.
name|rank
argument_list|,
name|mx
index|[
name|i
index|]
operator|.
name|score
argument_list|)
expr_stmt|;
block|}
name|free
argument_list|(
name|mx
argument_list|)
expr_stmt|;
name|debug_queue
argument_list|(
literal|"done detecting fuzzy"
argument_list|,
operator|&
name|outq
argument_list|)
expr_stmt|;
name|flush_rest
label|:
comment|/* At this point, we have found some renames and copies and they 	 * are kept in outq.  The original list is still in *q. 	 * 	 * Scan the original list and move them into the outq; we will sort 	 * outq and swap it into the queue supplied to pass that to 	 * downstream, so we assign the sort keys in this loop. 	 * 	 * See comments at the top of record_rename_pair for numbers used 	 * to assign rename_rank. 	 */
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
name|struct
name|diff_filepair
modifier|*
name|dp
decl_stmt|,
modifier|*
name|p
init|=
name|q
operator|->
name|queue
index|[
name|i
index|]
decl_stmt|;
if|if
condition|(
operator|!
name|DIFF_FILE_VALID
argument_list|(
name|p
operator|->
name|one
argument_list|)
condition|)
block|{
comment|/* creation or unmerged entries */
name|dp
operator|=
name|diff_queue
argument_list|(
operator|&
name|outq
argument_list|,
name|p
operator|->
name|one
argument_list|,
name|p
operator|->
name|two
argument_list|)
expr_stmt|;
name|dp
operator|->
name|rename_rank
operator|=
literal|4
expr_stmt|;
block|}
elseif|else
if|if
condition|(
operator|!
name|DIFF_FILE_VALID
argument_list|(
name|p
operator|->
name|two
argument_list|)
condition|)
block|{
comment|/* deletion */
name|dp
operator|=
name|diff_queue
argument_list|(
operator|&
name|outq
argument_list|,
name|p
operator|->
name|one
argument_list|,
name|p
operator|->
name|two
argument_list|)
expr_stmt|;
name|dp
operator|->
name|rename_rank
operator|=
literal|2
expr_stmt|;
block|}
else|else
block|{
comment|/* modification, or stay as is */
name|dp
operator|=
name|diff_queue
argument_list|(
operator|&
name|outq
argument_list|,
name|p
operator|->
name|one
argument_list|,
name|p
operator|->
name|two
argument_list|)
expr_stmt|;
name|dp
operator|->
name|rename_rank
operator|=
literal|4
expr_stmt|;
block|}
name|free
argument_list|(
name|p
argument_list|)
expr_stmt|;
block|}
name|debug_queue
argument_list|(
literal|"done copying original"
argument_list|,
operator|&
name|outq
argument_list|)
expr_stmt|;
comment|/* Sort outq */
name|qsort
argument_list|(
name|outq
operator|.
name|queue
argument_list|,
name|outq
operator|.
name|nr
argument_list|,
sizeof|sizeof
argument_list|(
name|outq
operator|.
name|queue
index|[
literal|0
index|]
argument_list|)
argument_list|,
name|rank_compare
argument_list|)
expr_stmt|;
name|debug_queue
argument_list|(
literal|"done sorting"
argument_list|,
operator|&
name|outq
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|q
operator|->
name|queue
argument_list|)
expr_stmt|;
name|q
operator|->
name|nr
operator|=
name|q
operator|->
name|alloc
operator|=
literal|0
expr_stmt|;
name|q
operator|->
name|queue
operator|=
name|NULL
expr_stmt|;
comment|/* Copy it out to q, removing duplicates. */
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|outq
operator|.
name|nr
condition|;
name|i
operator|++
control|)
block|{
name|struct
name|diff_filepair
modifier|*
name|p
init|=
name|outq
operator|.
name|queue
index|[
name|i
index|]
decl_stmt|;
if|if
condition|(
operator|!
name|DIFF_FILE_VALID
argument_list|(
name|p
operator|->
name|one
argument_list|)
condition|)
block|{
comment|/* created or unmerged */
if|if
condition|(
name|p
operator|->
name|two
operator|->
name|xfrm_flags
operator|&
name|RENAME_DST_MATCHED
condition|)
empty_stmt|;
comment|/* rename/copy created it already */
else|else
name|diff_queue
argument_list|(
name|q
argument_list|,
name|p
operator|->
name|one
argument_list|,
name|p
operator|->
name|two
argument_list|)
expr_stmt|;
block|}
elseif|else
if|if
condition|(
operator|!
name|DIFF_FILE_VALID
argument_list|(
name|p
operator|->
name|two
argument_list|)
condition|)
block|{
comment|/* deleted */
name|diff_queue
argument_list|(
name|q
argument_list|,
name|p
operator|->
name|one
argument_list|,
name|p
operator|->
name|two
argument_list|)
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|strcmp
argument_list|(
name|p
operator|->
name|one
operator|->
name|path
argument_list|,
name|p
operator|->
name|two
operator|->
name|path
argument_list|)
condition|)
block|{
comment|/* rename or copy */
name|struct
name|diff_filepair
modifier|*
name|dp
init|=
name|diff_queue
argument_list|(
name|q
argument_list|,
name|p
operator|->
name|one
argument_list|,
name|p
operator|->
name|two
argument_list|)
decl_stmt|;
name|dp
operator|->
name|score
operator|=
name|p
operator|->
name|score
expr_stmt|;
block|}
else|else
comment|/* otherwise it is a modified (or "stay") entry */
name|diff_queue
argument_list|(
name|q
argument_list|,
name|p
operator|->
name|one
argument_list|,
name|p
operator|->
name|two
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|p
argument_list|)
expr_stmt|;
block|}
name|free
argument_list|(
name|outq
operator|.
name|queue
argument_list|)
expr_stmt|;
name|debug_queue
argument_list|(
literal|"done collapsing"
argument_list|,
name|q
argument_list|)
expr_stmt|;
name|cleanup
label|:
name|free
argument_list|(
name|created
operator|.
name|s
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|deleted
operator|.
name|s
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|stay
operator|.
name|s
argument_list|)
expr_stmt|;
return|return;
block|}
end_function
end_unit
