begin_unit
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
file|"commit.h"
end_include
begin_include
include|#
directive|include
file|"tag.h"
end_include
begin_include
include|#
directive|include
file|"graph.h"
end_include
begin_include
include|#
directive|include
file|"log-tree.h"
end_include
begin_include
include|#
directive|include
file|"reflog-walk.h"
end_include
begin_include
include|#
directive|include
file|"refs.h"
end_include
begin_decl_stmt
DECL|variable|name_decoration
name|struct
name|decoration
name|name_decoration
init|=
block|{
literal|"object names"
block|}
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|add_name_decoration
specifier|static
name|void
name|add_name_decoration
parameter_list|(
specifier|const
name|char
modifier|*
name|prefix
parameter_list|,
specifier|const
name|char
modifier|*
name|name
parameter_list|,
name|struct
name|object
modifier|*
name|obj
parameter_list|)
block|{
name|int
name|plen
init|=
name|strlen
argument_list|(
name|prefix
argument_list|)
decl_stmt|;
name|int
name|nlen
init|=
name|strlen
argument_list|(
name|name
argument_list|)
decl_stmt|;
name|struct
name|name_decoration
modifier|*
name|res
init|=
name|xmalloc
argument_list|(
sizeof|sizeof
argument_list|(
expr|struct
name|name_decoration
argument_list|)
operator|+
name|plen
operator|+
name|nlen
argument_list|)
decl_stmt|;
name|memcpy
argument_list|(
name|res
operator|->
name|name
argument_list|,
name|prefix
argument_list|,
name|plen
argument_list|)
expr_stmt|;
name|memcpy
argument_list|(
name|res
operator|->
name|name
operator|+
name|plen
argument_list|,
name|name
argument_list|,
name|nlen
operator|+
literal|1
argument_list|)
expr_stmt|;
name|res
operator|->
name|next
operator|=
name|add_decoration
argument_list|(
operator|&
name|name_decoration
argument_list|,
name|obj
argument_list|,
name|res
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|add_ref_decoration
specifier|static
name|int
name|add_ref_decoration
parameter_list|(
specifier|const
name|char
modifier|*
name|refname
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|,
name|int
name|flags
parameter_list|,
name|void
modifier|*
name|cb_data
parameter_list|)
block|{
name|struct
name|object
modifier|*
name|obj
init|=
name|parse_object
argument_list|(
name|sha1
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
name|obj
condition|)
return|return
literal|0
return|;
name|add_name_decoration
argument_list|(
literal|""
argument_list|,
name|refname
argument_list|,
name|obj
argument_list|)
expr_stmt|;
while|while
condition|(
name|obj
operator|->
name|type
operator|==
name|OBJ_TAG
condition|)
block|{
name|obj
operator|=
operator|(
operator|(
expr|struct
name|tag
operator|*
operator|)
name|obj
operator|)
operator|->
name|tagged
expr_stmt|;
if|if
condition|(
operator|!
name|obj
condition|)
break|break;
name|add_name_decoration
argument_list|(
literal|"tag: "
argument_list|,
name|refname
argument_list|,
name|obj
argument_list|)
expr_stmt|;
block|}
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|load_ref_decorations
name|void
name|load_ref_decorations
parameter_list|(
name|void
parameter_list|)
block|{
specifier|static
name|int
name|loaded
decl_stmt|;
if|if
condition|(
operator|!
name|loaded
condition|)
block|{
name|loaded
operator|=
literal|1
expr_stmt|;
name|for_each_ref
argument_list|(
name|add_ref_decoration
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|show_parents
specifier|static
name|void
name|show_parents
parameter_list|(
name|struct
name|commit
modifier|*
name|commit
parameter_list|,
name|int
name|abbrev
parameter_list|)
block|{
name|struct
name|commit_list
modifier|*
name|p
decl_stmt|;
for|for
control|(
name|p
operator|=
name|commit
operator|->
name|parents
init|;
name|p
condition|;
name|p
operator|=
name|p
operator|->
name|next
control|)
block|{
name|struct
name|commit
modifier|*
name|parent
init|=
name|p
operator|->
name|item
decl_stmt|;
name|printf
argument_list|(
literal|" %s"
argument_list|,
name|diff_unique_abbrev
argument_list|(
name|parent
operator|->
name|object
operator|.
name|sha1
argument_list|,
name|abbrev
argument_list|)
argument_list|)
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|show_decorations
name|void
name|show_decorations
parameter_list|(
name|struct
name|commit
modifier|*
name|commit
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|prefix
decl_stmt|;
name|struct
name|name_decoration
modifier|*
name|decoration
decl_stmt|;
name|decoration
operator|=
name|lookup_decoration
argument_list|(
operator|&
name|name_decoration
argument_list|,
operator|&
name|commit
operator|->
name|object
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|decoration
condition|)
return|return;
name|prefix
operator|=
literal|" ("
expr_stmt|;
while|while
condition|(
name|decoration
condition|)
block|{
name|printf
argument_list|(
literal|"%s%s"
argument_list|,
name|prefix
argument_list|,
name|decoration
operator|->
name|name
argument_list|)
expr_stmt|;
name|prefix
operator|=
literal|", "
expr_stmt|;
name|decoration
operator|=
name|decoration
operator|->
name|next
expr_stmt|;
block|}
name|putchar
argument_list|(
literal|')'
argument_list|)
expr_stmt|;
block|}
end_function
begin_comment
comment|/*  * Search for "^[-A-Za-z]+: [^@]+@" pattern. It usually matches  * Signed-off-by: and Acked-by: lines.  */
end_comment
begin_function
DECL|function|detect_any_signoff
specifier|static
name|int
name|detect_any_signoff
parameter_list|(
name|char
modifier|*
name|letter
parameter_list|,
name|int
name|size
parameter_list|)
block|{
name|char
name|ch
decl_stmt|,
modifier|*
name|cp
decl_stmt|;
name|int
name|seen_colon
init|=
literal|0
decl_stmt|;
name|int
name|seen_at
init|=
literal|0
decl_stmt|;
name|int
name|seen_name
init|=
literal|0
decl_stmt|;
name|int
name|seen_head
init|=
literal|0
decl_stmt|;
name|cp
operator|=
name|letter
operator|+
name|size
expr_stmt|;
while|while
condition|(
name|letter
operator|<=
operator|--
name|cp
operator|&&
operator|(
name|ch
operator|=
operator|*
name|cp
operator|)
operator|==
literal|'\n'
condition|)
continue|continue;
while|while
condition|(
name|letter
operator|<=
name|cp
condition|)
block|{
name|ch
operator|=
operator|*
name|cp
operator|--
expr_stmt|;
if|if
condition|(
name|ch
operator|==
literal|'\n'
condition|)
break|break;
if|if
condition|(
operator|!
name|seen_at
condition|)
block|{
if|if
condition|(
name|ch
operator|==
literal|'@'
condition|)
name|seen_at
operator|=
literal|1
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|seen_colon
condition|)
block|{
if|if
condition|(
name|ch
operator|==
literal|'@'
condition|)
return|return
literal|0
return|;
elseif|else
if|if
condition|(
name|ch
operator|==
literal|':'
condition|)
name|seen_colon
operator|=
literal|1
expr_stmt|;
else|else
name|seen_name
operator|=
literal|1
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|(
literal|'A'
operator|<=
name|ch
operator|&&
name|ch
operator|<=
literal|'Z'
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
name|ch
operator|==
literal|'-'
condition|)
block|{
name|seen_head
operator|=
literal|1
expr_stmt|;
continue|continue;
block|}
comment|/* no empty last line doesn't match */
return|return
literal|0
return|;
block|}
return|return
name|seen_head
operator|&&
name|seen_name
return|;
block|}
end_function
begin_function
DECL|function|append_signoff
specifier|static
name|void
name|append_signoff
parameter_list|(
name|struct
name|strbuf
modifier|*
name|sb
parameter_list|,
specifier|const
name|char
modifier|*
name|signoff
parameter_list|)
block|{
specifier|static
specifier|const
name|char
name|signed_off_by
index|[]
init|=
literal|"Signed-off-by: "
decl_stmt|;
name|size_t
name|signoff_len
init|=
name|strlen
argument_list|(
name|signoff
argument_list|)
decl_stmt|;
name|int
name|has_signoff
init|=
literal|0
decl_stmt|;
name|char
modifier|*
name|cp
decl_stmt|;
name|cp
operator|=
name|sb
operator|->
name|buf
expr_stmt|;
comment|/* First see if we already have the sign-off by the signer */
while|while
condition|(
operator|(
name|cp
operator|=
name|strstr
argument_list|(
name|cp
argument_list|,
name|signed_off_by
argument_list|)
operator|)
condition|)
block|{
name|has_signoff
operator|=
literal|1
expr_stmt|;
name|cp
operator|+=
name|strlen
argument_list|(
name|signed_off_by
argument_list|)
expr_stmt|;
if|if
condition|(
name|cp
operator|+
name|signoff_len
operator|>=
name|sb
operator|->
name|buf
operator|+
name|sb
operator|->
name|len
condition|)
break|break;
if|if
condition|(
name|strncmp
argument_list|(
name|cp
argument_list|,
name|signoff
argument_list|,
name|signoff_len
argument_list|)
condition|)
continue|continue;
if|if
condition|(
operator|!
name|isspace
argument_list|(
name|cp
index|[
name|signoff_len
index|]
argument_list|)
condition|)
continue|continue;
comment|/* we already have him */
return|return;
block|}
if|if
condition|(
operator|!
name|has_signoff
condition|)
name|has_signoff
operator|=
name|detect_any_signoff
argument_list|(
name|sb
operator|->
name|buf
argument_list|,
name|sb
operator|->
name|len
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|has_signoff
condition|)
name|strbuf_addch
argument_list|(
name|sb
argument_list|,
literal|'\n'
argument_list|)
expr_stmt|;
name|strbuf_addstr
argument_list|(
name|sb
argument_list|,
name|signed_off_by
argument_list|)
expr_stmt|;
name|strbuf_add
argument_list|(
name|sb
argument_list|,
name|signoff
argument_list|,
name|signoff_len
argument_list|)
expr_stmt|;
name|strbuf_addch
argument_list|(
name|sb
argument_list|,
literal|'\n'
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|digits_in_number
specifier|static
name|unsigned
name|int
name|digits_in_number
parameter_list|(
name|unsigned
name|int
name|number
parameter_list|)
block|{
name|unsigned
name|int
name|i
init|=
literal|10
decl_stmt|,
name|result
init|=
literal|1
decl_stmt|;
while|while
condition|(
name|i
operator|<=
name|number
condition|)
block|{
name|i
operator|*=
literal|10
expr_stmt|;
name|result
operator|++
expr_stmt|;
block|}
return|return
name|result
return|;
block|}
end_function
begin_function
DECL|function|has_non_ascii
specifier|static
name|int
name|has_non_ascii
parameter_list|(
specifier|const
name|char
modifier|*
name|s
parameter_list|)
block|{
name|int
name|ch
decl_stmt|;
if|if
condition|(
operator|!
name|s
condition|)
return|return
literal|0
return|;
while|while
condition|(
operator|(
name|ch
operator|=
operator|*
name|s
operator|++
operator|)
operator|!=
literal|'\0'
condition|)
block|{
if|if
condition|(
name|non_ascii
argument_list|(
name|ch
argument_list|)
condition|)
return|return
literal|1
return|;
block|}
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|log_write_email_headers
name|void
name|log_write_email_headers
parameter_list|(
name|struct
name|rev_info
modifier|*
name|opt
parameter_list|,
specifier|const
name|char
modifier|*
name|name
parameter_list|,
specifier|const
name|char
modifier|*
modifier|*
name|subject_p
parameter_list|,
specifier|const
name|char
modifier|*
modifier|*
name|extra_headers_p
parameter_list|,
name|int
modifier|*
name|need_8bit_cte_p
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|subject
init|=
name|NULL
decl_stmt|;
specifier|const
name|char
modifier|*
name|extra_headers
init|=
name|opt
operator|->
name|extra_headers
decl_stmt|;
operator|*
name|need_8bit_cte_p
operator|=
literal|0
expr_stmt|;
comment|/* unknown */
if|if
condition|(
name|opt
operator|->
name|total
operator|>
literal|0
condition|)
block|{
specifier|static
name|char
name|buffer
index|[
literal|64
index|]
decl_stmt|;
name|snprintf
argument_list|(
name|buffer
argument_list|,
sizeof|sizeof
argument_list|(
name|buffer
argument_list|)
argument_list|,
literal|"Subject: [%s %0*d/%d] "
argument_list|,
name|opt
operator|->
name|subject_prefix
argument_list|,
name|digits_in_number
argument_list|(
name|opt
operator|->
name|total
argument_list|)
argument_list|,
name|opt
operator|->
name|nr
argument_list|,
name|opt
operator|->
name|total
argument_list|)
expr_stmt|;
name|subject
operator|=
name|buffer
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|opt
operator|->
name|total
operator|==
literal|0
operator|&&
name|opt
operator|->
name|subject_prefix
operator|&&
operator|*
name|opt
operator|->
name|subject_prefix
condition|)
block|{
specifier|static
name|char
name|buffer
index|[
literal|256
index|]
decl_stmt|;
name|snprintf
argument_list|(
name|buffer
argument_list|,
sizeof|sizeof
argument_list|(
name|buffer
argument_list|)
argument_list|,
literal|"Subject: [%s] "
argument_list|,
name|opt
operator|->
name|subject_prefix
argument_list|)
expr_stmt|;
name|subject
operator|=
name|buffer
expr_stmt|;
block|}
else|else
block|{
name|subject
operator|=
literal|"Subject: "
expr_stmt|;
block|}
name|printf
argument_list|(
literal|"From %s Mon Sep 17 00:00:00 2001\n"
argument_list|,
name|name
argument_list|)
expr_stmt|;
name|graph_show_oneline
argument_list|(
name|opt
operator|->
name|graph
argument_list|)
expr_stmt|;
if|if
condition|(
name|opt
operator|->
name|message_id
condition|)
block|{
name|printf
argument_list|(
literal|"Message-Id:<%s>\n"
argument_list|,
name|opt
operator|->
name|message_id
argument_list|)
expr_stmt|;
name|graph_show_oneline
argument_list|(
name|opt
operator|->
name|graph
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|opt
operator|->
name|ref_message_id
condition|)
block|{
name|printf
argument_list|(
literal|"In-Reply-To:<%s>\nReferences:<%s>\n"
argument_list|,
name|opt
operator|->
name|ref_message_id
argument_list|,
name|opt
operator|->
name|ref_message_id
argument_list|)
expr_stmt|;
name|graph_show_oneline
argument_list|(
name|opt
operator|->
name|graph
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|opt
operator|->
name|mime_boundary
condition|)
block|{
specifier|static
name|char
name|subject_buffer
index|[
literal|1024
index|]
decl_stmt|;
specifier|static
name|char
name|buffer
index|[
literal|1024
index|]
decl_stmt|;
operator|*
name|need_8bit_cte_p
operator|=
operator|-
literal|1
expr_stmt|;
comment|/* NEVER */
name|snprintf
argument_list|(
name|subject_buffer
argument_list|,
sizeof|sizeof
argument_list|(
name|subject_buffer
argument_list|)
operator|-
literal|1
argument_list|,
literal|"%s"
literal|"MIME-Version: 1.0\n"
literal|"Content-Type: multipart/mixed;"
literal|" boundary=\"%s%s\"\n"
literal|"\n"
literal|"This is a multi-part message in MIME "
literal|"format.\n"
literal|"--%s%s\n"
literal|"Content-Type: text/plain; "
literal|"charset=UTF-8; format=fixed\n"
literal|"Content-Transfer-Encoding: 8bit\n\n"
argument_list|,
name|extra_headers
condition|?
name|extra_headers
else|:
literal|""
argument_list|,
name|mime_boundary_leader
argument_list|,
name|opt
operator|->
name|mime_boundary
argument_list|,
name|mime_boundary_leader
argument_list|,
name|opt
operator|->
name|mime_boundary
argument_list|)
expr_stmt|;
name|extra_headers
operator|=
name|subject_buffer
expr_stmt|;
name|snprintf
argument_list|(
name|buffer
argument_list|,
sizeof|sizeof
argument_list|(
name|buffer
argument_list|)
operator|-
literal|1
argument_list|,
literal|"\n--%s%s\n"
literal|"Content-Type: text/x-patch;"
literal|" name=\"%s.diff\"\n"
literal|"Content-Transfer-Encoding: 8bit\n"
literal|"Content-Disposition: %s;"
literal|" filename=\"%s.diff\"\n\n"
argument_list|,
name|mime_boundary_leader
argument_list|,
name|opt
operator|->
name|mime_boundary
argument_list|,
name|name
argument_list|,
name|opt
operator|->
name|no_inline
condition|?
literal|"attachment"
else|:
literal|"inline"
argument_list|,
name|name
argument_list|)
expr_stmt|;
name|opt
operator|->
name|diffopt
operator|.
name|stat_sep
operator|=
name|buffer
expr_stmt|;
block|}
operator|*
name|subject_p
operator|=
name|subject
expr_stmt|;
operator|*
name|extra_headers_p
operator|=
name|extra_headers
expr_stmt|;
block|}
end_function
begin_function
DECL|function|show_log
name|void
name|show_log
parameter_list|(
name|struct
name|rev_info
modifier|*
name|opt
parameter_list|)
block|{
name|struct
name|strbuf
name|msgbuf
decl_stmt|;
name|struct
name|log_info
modifier|*
name|log
init|=
name|opt
operator|->
name|loginfo
decl_stmt|;
name|struct
name|commit
modifier|*
name|commit
init|=
name|log
operator|->
name|commit
decl_stmt|,
modifier|*
name|parent
init|=
name|log
operator|->
name|parent
decl_stmt|;
name|int
name|abbrev
init|=
name|opt
operator|->
name|diffopt
operator|.
name|abbrev
decl_stmt|;
name|int
name|abbrev_commit
init|=
name|opt
operator|->
name|abbrev_commit
condition|?
name|opt
operator|->
name|abbrev
else|:
literal|40
decl_stmt|;
specifier|const
name|char
modifier|*
name|subject
init|=
name|NULL
decl_stmt|,
modifier|*
name|extra_headers
init|=
name|opt
operator|->
name|extra_headers
decl_stmt|;
name|int
name|need_8bit_cte
init|=
literal|0
decl_stmt|;
name|opt
operator|->
name|loginfo
operator|=
name|NULL
expr_stmt|;
if|if
condition|(
operator|!
name|opt
operator|->
name|verbose_header
condition|)
block|{
name|graph_show_commit
argument_list|(
name|opt
operator|->
name|graph
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|opt
operator|->
name|graph
condition|)
block|{
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
name|putchar
argument_list|(
literal|'-'
argument_list|)
expr_stmt|;
elseif|else
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
name|putchar
argument_list|(
literal|'^'
argument_list|)
expr_stmt|;
elseif|else
if|if
condition|(
name|opt
operator|->
name|left_right
condition|)
block|{
if|if
condition|(
name|commit
operator|->
name|object
operator|.
name|flags
operator|&
name|SYMMETRIC_LEFT
condition|)
name|putchar
argument_list|(
literal|'<'
argument_list|)
expr_stmt|;
else|else
name|putchar
argument_list|(
literal|'>'
argument_list|)
expr_stmt|;
block|}
block|}
name|fputs
argument_list|(
name|diff_unique_abbrev
argument_list|(
name|commit
operator|->
name|object
operator|.
name|sha1
argument_list|,
name|abbrev_commit
argument_list|)
argument_list|,
name|stdout
argument_list|)
expr_stmt|;
if|if
condition|(
name|opt
operator|->
name|print_parents
condition|)
name|show_parents
argument_list|(
name|commit
argument_list|,
name|abbrev_commit
argument_list|)
expr_stmt|;
name|show_decorations
argument_list|(
name|commit
argument_list|)
expr_stmt|;
if|if
condition|(
name|opt
operator|->
name|graph
operator|&&
operator|!
name|graph_is_commit_finished
argument_list|(
name|opt
operator|->
name|graph
argument_list|)
condition|)
block|{
name|putchar
argument_list|(
literal|'\n'
argument_list|)
expr_stmt|;
name|graph_show_remainder
argument_list|(
name|opt
operator|->
name|graph
argument_list|)
expr_stmt|;
block|}
name|putchar
argument_list|(
name|opt
operator|->
name|diffopt
operator|.
name|line_termination
argument_list|)
expr_stmt|;
return|return;
block|}
comment|/* 	 * If use_terminator is set, add a newline at the end of the entry. 	 * Otherwise, add a diffopt.line_termination character before all 	 * entries but the first.  (IOW, as a separator between entries) 	 */
if|if
condition|(
name|opt
operator|->
name|shown_one
operator|&&
operator|!
name|opt
operator|->
name|use_terminator
condition|)
block|{
comment|/* 		 * If entries are separated by a newline, the output 		 * should look human-readable.  If the last entry ended 		 * with a newline, print the graph output before this 		 * newline.  Otherwise it will end up as a completely blank 		 * line and will look like a gap in the graph. 		 * 		 * If the entry separator is not a newline, the output is 		 * primarily intended for programmatic consumption, and we 		 * never want the extra graph output before the entry 		 * separator. 		 */
if|if
condition|(
name|opt
operator|->
name|diffopt
operator|.
name|line_termination
operator|==
literal|'\n'
operator|&&
operator|!
name|opt
operator|->
name|missing_newline
condition|)
name|graph_show_padding
argument_list|(
name|opt
operator|->
name|graph
argument_list|)
expr_stmt|;
name|putchar
argument_list|(
name|opt
operator|->
name|diffopt
operator|.
name|line_termination
argument_list|)
expr_stmt|;
block|}
name|opt
operator|->
name|shown_one
operator|=
literal|1
expr_stmt|;
comment|/* 	 * If the history graph was requested, 	 * print the graph, up to this commit's line 	 */
name|graph_show_commit
argument_list|(
name|opt
operator|->
name|graph
argument_list|)
expr_stmt|;
comment|/* 	 * Print header line of header.. 	 */
if|if
condition|(
name|opt
operator|->
name|commit_format
operator|==
name|CMIT_FMT_EMAIL
condition|)
block|{
name|log_write_email_headers
argument_list|(
name|opt
argument_list|,
name|sha1_to_hex
argument_list|(
name|commit
operator|->
name|object
operator|.
name|sha1
argument_list|)
argument_list|,
operator|&
name|subject
argument_list|,
operator|&
name|extra_headers
argument_list|,
operator|&
name|need_8bit_cte
argument_list|)
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|opt
operator|->
name|commit_format
operator|!=
name|CMIT_FMT_USERFORMAT
condition|)
block|{
name|fputs
argument_list|(
name|diff_get_color_opt
argument_list|(
operator|&
name|opt
operator|->
name|diffopt
argument_list|,
name|DIFF_COMMIT
argument_list|)
argument_list|,
name|stdout
argument_list|)
expr_stmt|;
if|if
condition|(
name|opt
operator|->
name|commit_format
operator|!=
name|CMIT_FMT_ONELINE
condition|)
name|fputs
argument_list|(
literal|"commit "
argument_list|,
name|stdout
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|opt
operator|->
name|graph
condition|)
block|{
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
name|putchar
argument_list|(
literal|'-'
argument_list|)
expr_stmt|;
elseif|else
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
name|putchar
argument_list|(
literal|'^'
argument_list|)
expr_stmt|;
elseif|else
if|if
condition|(
name|opt
operator|->
name|left_right
condition|)
block|{
if|if
condition|(
name|commit
operator|->
name|object
operator|.
name|flags
operator|&
name|SYMMETRIC_LEFT
condition|)
name|putchar
argument_list|(
literal|'<'
argument_list|)
expr_stmt|;
else|else
name|putchar
argument_list|(
literal|'>'
argument_list|)
expr_stmt|;
block|}
block|}
name|fputs
argument_list|(
name|diff_unique_abbrev
argument_list|(
name|commit
operator|->
name|object
operator|.
name|sha1
argument_list|,
name|abbrev_commit
argument_list|)
argument_list|,
name|stdout
argument_list|)
expr_stmt|;
if|if
condition|(
name|opt
operator|->
name|print_parents
condition|)
name|show_parents
argument_list|(
name|commit
argument_list|,
name|abbrev_commit
argument_list|)
expr_stmt|;
if|if
condition|(
name|parent
condition|)
name|printf
argument_list|(
literal|" (from %s)"
argument_list|,
name|diff_unique_abbrev
argument_list|(
name|parent
operator|->
name|object
operator|.
name|sha1
argument_list|,
name|abbrev_commit
argument_list|)
argument_list|)
expr_stmt|;
name|show_decorations
argument_list|(
name|commit
argument_list|)
expr_stmt|;
name|printf
argument_list|(
literal|"%s"
argument_list|,
name|diff_get_color_opt
argument_list|(
operator|&
name|opt
operator|->
name|diffopt
argument_list|,
name|DIFF_RESET
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
name|opt
operator|->
name|commit_format
operator|==
name|CMIT_FMT_ONELINE
condition|)
block|{
name|putchar
argument_list|(
literal|' '
argument_list|)
expr_stmt|;
block|}
else|else
block|{
name|putchar
argument_list|(
literal|'\n'
argument_list|)
expr_stmt|;
name|graph_show_oneline
argument_list|(
name|opt
operator|->
name|graph
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|opt
operator|->
name|reflog_info
condition|)
block|{
comment|/* 			 * setup_revisions() ensures that opt->reflog_info 			 * and opt->graph cannot both be set, 			 * so we don't need to worry about printing the 			 * graph info here. 			 */
name|show_reflog_message
argument_list|(
name|opt
operator|->
name|reflog_info
argument_list|,
name|opt
operator|->
name|commit_format
operator|==
name|CMIT_FMT_ONELINE
argument_list|,
name|opt
operator|->
name|date_mode
argument_list|)
expr_stmt|;
if|if
condition|(
name|opt
operator|->
name|commit_format
operator|==
name|CMIT_FMT_ONELINE
condition|)
return|return;
block|}
block|}
if|if
condition|(
operator|!
name|commit
operator|->
name|buffer
condition|)
return|return;
comment|/* 	 * And then the pretty-printed message itself 	 */
name|strbuf_init
argument_list|(
operator|&
name|msgbuf
argument_list|,
literal|0
argument_list|)
expr_stmt|;
if|if
condition|(
name|need_8bit_cte
operator|>=
literal|0
condition|)
name|need_8bit_cte
operator|=
name|has_non_ascii
argument_list|(
name|opt
operator|->
name|add_signoff
argument_list|)
expr_stmt|;
name|pretty_print_commit
argument_list|(
name|opt
operator|->
name|commit_format
argument_list|,
name|commit
argument_list|,
operator|&
name|msgbuf
argument_list|,
name|abbrev
argument_list|,
name|subject
argument_list|,
name|extra_headers
argument_list|,
name|opt
operator|->
name|date_mode
argument_list|,
name|need_8bit_cte
argument_list|)
expr_stmt|;
if|if
condition|(
name|opt
operator|->
name|add_signoff
condition|)
name|append_signoff
argument_list|(
operator|&
name|msgbuf
argument_list|,
name|opt
operator|->
name|add_signoff
argument_list|)
expr_stmt|;
if|if
condition|(
name|opt
operator|->
name|show_log_size
condition|)
block|{
name|printf
argument_list|(
literal|"log size %i\n"
argument_list|,
operator|(
name|int
operator|)
name|msgbuf
operator|.
name|len
argument_list|)
expr_stmt|;
name|graph_show_oneline
argument_list|(
name|opt
operator|->
name|graph
argument_list|)
expr_stmt|;
block|}
comment|/* 	 * Set opt->missing_newline if msgbuf doesn't 	 * end in a newline (including if it is empty) 	 */
if|if
condition|(
operator|!
name|msgbuf
operator|.
name|len
operator|||
name|msgbuf
operator|.
name|buf
index|[
name|msgbuf
operator|.
name|len
operator|-
literal|1
index|]
operator|!=
literal|'\n'
condition|)
name|opt
operator|->
name|missing_newline
operator|=
literal|1
expr_stmt|;
else|else
name|opt
operator|->
name|missing_newline
operator|=
literal|0
expr_stmt|;
if|if
condition|(
name|opt
operator|->
name|graph
condition|)
name|graph_show_commit_msg
argument_list|(
name|opt
operator|->
name|graph
argument_list|,
operator|&
name|msgbuf
argument_list|)
expr_stmt|;
else|else
name|fwrite
argument_list|(
name|msgbuf
operator|.
name|buf
argument_list|,
sizeof|sizeof
argument_list|(
name|char
argument_list|)
argument_list|,
name|msgbuf
operator|.
name|len
argument_list|,
name|stdout
argument_list|)
expr_stmt|;
if|if
condition|(
name|opt
operator|->
name|use_terminator
condition|)
block|{
if|if
condition|(
operator|!
name|opt
operator|->
name|missing_newline
condition|)
name|graph_show_padding
argument_list|(
name|opt
operator|->
name|graph
argument_list|)
expr_stmt|;
name|putchar
argument_list|(
literal|'\n'
argument_list|)
expr_stmt|;
block|}
name|strbuf_release
argument_list|(
operator|&
name|msgbuf
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|log_tree_diff_flush
name|int
name|log_tree_diff_flush
parameter_list|(
name|struct
name|rev_info
modifier|*
name|opt
parameter_list|)
block|{
name|diffcore_std
argument_list|(
operator|&
name|opt
operator|->
name|diffopt
argument_list|)
expr_stmt|;
if|if
condition|(
name|diff_queue_is_empty
argument_list|()
condition|)
block|{
name|int
name|saved_fmt
init|=
name|opt
operator|->
name|diffopt
operator|.
name|output_format
decl_stmt|;
name|opt
operator|->
name|diffopt
operator|.
name|output_format
operator|=
name|DIFF_FORMAT_NO_OUTPUT
expr_stmt|;
name|diff_flush
argument_list|(
operator|&
name|opt
operator|->
name|diffopt
argument_list|)
expr_stmt|;
name|opt
operator|->
name|diffopt
operator|.
name|output_format
operator|=
name|saved_fmt
expr_stmt|;
return|return
literal|0
return|;
block|}
if|if
condition|(
name|opt
operator|->
name|loginfo
operator|&&
operator|!
name|opt
operator|->
name|no_commit_id
condition|)
block|{
comment|/* When showing a verbose header (i.e. log message), 		 * and not in --pretty=oneline format, we would want 		 * an extra newline between the end of log and the 		 * output for readability. 		 */
name|show_log
argument_list|(
name|opt
argument_list|)
expr_stmt|;
if|if
condition|(
operator|(
name|opt
operator|->
name|diffopt
operator|.
name|output_format
operator|&
operator|~
name|DIFF_FORMAT_NO_OUTPUT
operator|)
operator|&&
name|opt
operator|->
name|verbose_header
operator|&&
name|opt
operator|->
name|commit_format
operator|!=
name|CMIT_FMT_ONELINE
condition|)
block|{
name|int
name|pch
init|=
name|DIFF_FORMAT_DIFFSTAT
operator||
name|DIFF_FORMAT_PATCH
decl_stmt|;
if|if
condition|(
operator|(
name|pch
operator|&
name|opt
operator|->
name|diffopt
operator|.
name|output_format
operator|)
operator|==
name|pch
condition|)
name|printf
argument_list|(
literal|"---"
argument_list|)
expr_stmt|;
name|putchar
argument_list|(
literal|'\n'
argument_list|)
expr_stmt|;
block|}
block|}
name|diff_flush
argument_list|(
operator|&
name|opt
operator|->
name|diffopt
argument_list|)
expr_stmt|;
return|return
literal|1
return|;
block|}
end_function
begin_function
DECL|function|do_diff_combined
specifier|static
name|int
name|do_diff_combined
parameter_list|(
name|struct
name|rev_info
modifier|*
name|opt
parameter_list|,
name|struct
name|commit
modifier|*
name|commit
parameter_list|)
block|{
name|unsigned
specifier|const
name|char
modifier|*
name|sha1
init|=
name|commit
operator|->
name|object
operator|.
name|sha1
decl_stmt|;
name|diff_tree_combined_merge
argument_list|(
name|sha1
argument_list|,
name|opt
operator|->
name|dense_combined_merges
argument_list|,
name|opt
argument_list|)
expr_stmt|;
return|return
operator|!
name|opt
operator|->
name|loginfo
return|;
block|}
end_function
begin_comment
comment|/*  * Show the diff of a commit.  *  * Return true if we printed any log info messages  */
end_comment
begin_function
DECL|function|log_tree_diff
specifier|static
name|int
name|log_tree_diff
parameter_list|(
name|struct
name|rev_info
modifier|*
name|opt
parameter_list|,
name|struct
name|commit
modifier|*
name|commit
parameter_list|,
name|struct
name|log_info
modifier|*
name|log
parameter_list|)
block|{
name|int
name|showed_log
decl_stmt|;
name|struct
name|commit_list
modifier|*
name|parents
decl_stmt|;
name|unsigned
specifier|const
name|char
modifier|*
name|sha1
init|=
name|commit
operator|->
name|object
operator|.
name|sha1
decl_stmt|;
if|if
condition|(
operator|!
name|opt
operator|->
name|diff
operator|&&
operator|!
name|DIFF_OPT_TST
argument_list|(
operator|&
name|opt
operator|->
name|diffopt
argument_list|,
name|EXIT_WITH_STATUS
argument_list|)
condition|)
return|return
literal|0
return|;
comment|/* Root commit? */
name|parents
operator|=
name|commit
operator|->
name|parents
expr_stmt|;
if|if
condition|(
operator|!
name|parents
condition|)
block|{
if|if
condition|(
name|opt
operator|->
name|show_root_diff
condition|)
block|{
name|diff_root_tree_sha1
argument_list|(
name|sha1
argument_list|,
literal|""
argument_list|,
operator|&
name|opt
operator|->
name|diffopt
argument_list|)
expr_stmt|;
name|log_tree_diff_flush
argument_list|(
name|opt
argument_list|)
expr_stmt|;
block|}
return|return
operator|!
name|opt
operator|->
name|loginfo
return|;
block|}
comment|/* More than one parent? */
if|if
condition|(
name|parents
operator|&&
name|parents
operator|->
name|next
condition|)
block|{
if|if
condition|(
name|opt
operator|->
name|ignore_merges
condition|)
return|return
literal|0
return|;
elseif|else
if|if
condition|(
name|opt
operator|->
name|combine_merges
condition|)
return|return
name|do_diff_combined
argument_list|(
name|opt
argument_list|,
name|commit
argument_list|)
return|;
comment|/* If we show individual diffs, show the parent info */
name|log
operator|->
name|parent
operator|=
name|parents
operator|->
name|item
expr_stmt|;
block|}
name|showed_log
operator|=
literal|0
expr_stmt|;
for|for
control|(
init|;
condition|;
control|)
block|{
name|struct
name|commit
modifier|*
name|parent
init|=
name|parents
operator|->
name|item
decl_stmt|;
name|diff_tree_sha1
argument_list|(
name|parent
operator|->
name|object
operator|.
name|sha1
argument_list|,
name|sha1
argument_list|,
literal|""
argument_list|,
operator|&
name|opt
operator|->
name|diffopt
argument_list|)
expr_stmt|;
name|log_tree_diff_flush
argument_list|(
name|opt
argument_list|)
expr_stmt|;
name|showed_log
operator||=
operator|!
name|opt
operator|->
name|loginfo
expr_stmt|;
comment|/* Set up the log info for the next parent, if any.. */
name|parents
operator|=
name|parents
operator|->
name|next
expr_stmt|;
if|if
condition|(
operator|!
name|parents
condition|)
break|break;
name|log
operator|->
name|parent
operator|=
name|parents
operator|->
name|item
expr_stmt|;
name|opt
operator|->
name|loginfo
operator|=
name|log
expr_stmt|;
block|}
return|return
name|showed_log
return|;
block|}
end_function
begin_function
DECL|function|log_tree_commit
name|int
name|log_tree_commit
parameter_list|(
name|struct
name|rev_info
modifier|*
name|opt
parameter_list|,
name|struct
name|commit
modifier|*
name|commit
parameter_list|)
block|{
name|struct
name|log_info
name|log
decl_stmt|;
name|int
name|shown
decl_stmt|;
name|log
operator|.
name|commit
operator|=
name|commit
expr_stmt|;
name|log
operator|.
name|parent
operator|=
name|NULL
expr_stmt|;
name|opt
operator|->
name|loginfo
operator|=
operator|&
name|log
expr_stmt|;
name|shown
operator|=
name|log_tree_diff
argument_list|(
name|opt
argument_list|,
name|commit
argument_list|,
operator|&
name|log
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|shown
operator|&&
name|opt
operator|->
name|loginfo
operator|&&
name|opt
operator|->
name|always_show_header
condition|)
block|{
name|log
operator|.
name|parent
operator|=
name|NULL
expr_stmt|;
name|show_log
argument_list|(
name|opt
argument_list|)
expr_stmt|;
name|shown
operator|=
literal|1
expr_stmt|;
block|}
name|opt
operator|->
name|loginfo
operator|=
name|NULL
expr_stmt|;
name|maybe_flush_or_die
argument_list|(
name|stdout
argument_list|,
literal|"stdout"
argument_list|)
expr_stmt|;
return|return
name|shown
return|;
block|}
end_function
end_unit
