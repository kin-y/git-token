begin_unit
begin_comment
comment|/*  *  LibXDiff by Davide Libenzi ( File Differential Library )  *  Copyright (C) 2003	Davide Libenzi  *  *  This library is free software; you can redistribute it and/or  *  modify it under the terms of the GNU Lesser General Public  *  License as published by the Free Software Foundation; either  *  version 2.1 of the License, or (at your option) any later version.  *  *  This library is distributed in the hope that it will be useful,  *  but WITHOUT ANY WARRANTY; without even the implied warranty of  *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU  *  Lesser General Public License for more details.  *  *  You should have received a copy of the GNU Lesser General Public  *  License along with this library; if not, write to the Free Software  *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA  *  *  Davide Libenzi<davidel@xmailserver.org>  *  */
end_comment
begin_include
include|#
directive|include
file|<limits.h>
end_include
begin_include
include|#
directive|include
file|<assert.h>
end_include
begin_include
include|#
directive|include
file|"xinclude.h"
end_include
begin_function
DECL|function|xdl_bogosqrt
name|long
name|xdl_bogosqrt
parameter_list|(
name|long
name|n
parameter_list|)
block|{
name|long
name|i
decl_stmt|;
comment|/* 	 * Classical integer square root approximation using shifts. 	 */
for|for
control|(
name|i
operator|=
literal|1
init|;
name|n
operator|>
literal|0
condition|;
name|n
operator|>>=
literal|2
control|)
name|i
operator|<<=
literal|1
expr_stmt|;
return|return
name|i
return|;
block|}
end_function
begin_function
DECL|function|xdl_emit_diffrec
name|int
name|xdl_emit_diffrec
parameter_list|(
name|char
specifier|const
modifier|*
name|rec
parameter_list|,
name|long
name|size
parameter_list|,
name|char
specifier|const
modifier|*
name|pre
parameter_list|,
name|long
name|psize
parameter_list|,
name|xdemitcb_t
modifier|*
name|ecb
parameter_list|)
block|{
name|int
name|i
init|=
literal|2
decl_stmt|;
name|mmbuffer_t
name|mb
index|[
literal|3
index|]
decl_stmt|;
name|mb
index|[
literal|0
index|]
operator|.
name|ptr
operator|=
operator|(
name|char
operator|*
operator|)
name|pre
expr_stmt|;
name|mb
index|[
literal|0
index|]
operator|.
name|size
operator|=
name|psize
expr_stmt|;
name|mb
index|[
literal|1
index|]
operator|.
name|ptr
operator|=
operator|(
name|char
operator|*
operator|)
name|rec
expr_stmt|;
name|mb
index|[
literal|1
index|]
operator|.
name|size
operator|=
name|size
expr_stmt|;
if|if
condition|(
name|size
operator|>
literal|0
operator|&&
name|rec
index|[
name|size
operator|-
literal|1
index|]
operator|!=
literal|'\n'
condition|)
block|{
name|mb
index|[
literal|2
index|]
operator|.
name|ptr
operator|=
operator|(
name|char
operator|*
operator|)
literal|"\n\\ No newline at end of file\n"
expr_stmt|;
name|mb
index|[
literal|2
index|]
operator|.
name|size
operator|=
name|strlen
argument_list|(
name|mb
index|[
literal|2
index|]
operator|.
name|ptr
argument_list|)
expr_stmt|;
name|i
operator|++
expr_stmt|;
block|}
if|if
condition|(
name|ecb
operator|->
name|outf
argument_list|(
name|ecb
operator|->
name|priv
argument_list|,
name|mb
argument_list|,
name|i
argument_list|)
operator|<
literal|0
condition|)
block|{
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
DECL|function|xdl_mmfile_first
name|void
modifier|*
name|xdl_mmfile_first
parameter_list|(
name|mmfile_t
modifier|*
name|mmf
parameter_list|,
name|long
modifier|*
name|size
parameter_list|)
block|{
operator|*
name|size
operator|=
name|mmf
operator|->
name|size
expr_stmt|;
return|return
name|mmf
operator|->
name|ptr
return|;
block|}
end_function
begin_function
DECL|function|xdl_mmfile_size
name|long
name|xdl_mmfile_size
parameter_list|(
name|mmfile_t
modifier|*
name|mmf
parameter_list|)
block|{
return|return
name|mmf
operator|->
name|size
return|;
block|}
end_function
begin_function
DECL|function|xdl_cha_init
name|int
name|xdl_cha_init
parameter_list|(
name|chastore_t
modifier|*
name|cha
parameter_list|,
name|long
name|isize
parameter_list|,
name|long
name|icount
parameter_list|)
block|{
name|cha
operator|->
name|head
operator|=
name|cha
operator|->
name|tail
operator|=
name|NULL
expr_stmt|;
name|cha
operator|->
name|isize
operator|=
name|isize
expr_stmt|;
name|cha
operator|->
name|nsize
operator|=
name|icount
operator|*
name|isize
expr_stmt|;
name|cha
operator|->
name|ancur
operator|=
name|cha
operator|->
name|sncur
operator|=
name|NULL
expr_stmt|;
name|cha
operator|->
name|scurr
operator|=
literal|0
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|xdl_cha_free
name|void
name|xdl_cha_free
parameter_list|(
name|chastore_t
modifier|*
name|cha
parameter_list|)
block|{
name|chanode_t
modifier|*
name|cur
decl_stmt|,
modifier|*
name|tmp
decl_stmt|;
for|for
control|(
name|cur
operator|=
name|cha
operator|->
name|head
init|;
operator|(
name|tmp
operator|=
name|cur
operator|)
operator|!=
name|NULL
condition|;
control|)
block|{
name|cur
operator|=
name|cur
operator|->
name|next
expr_stmt|;
name|xdl_free
argument_list|(
name|tmp
argument_list|)
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|xdl_cha_alloc
name|void
modifier|*
name|xdl_cha_alloc
parameter_list|(
name|chastore_t
modifier|*
name|cha
parameter_list|)
block|{
name|chanode_t
modifier|*
name|ancur
decl_stmt|;
name|void
modifier|*
name|data
decl_stmt|;
if|if
condition|(
operator|!
operator|(
name|ancur
operator|=
name|cha
operator|->
name|ancur
operator|)
operator|||
name|ancur
operator|->
name|icurr
operator|==
name|cha
operator|->
name|nsize
condition|)
block|{
if|if
condition|(
operator|!
operator|(
name|ancur
operator|=
operator|(
name|chanode_t
operator|*
operator|)
name|xdl_malloc
argument_list|(
sizeof|sizeof
argument_list|(
name|chanode_t
argument_list|)
operator|+
name|cha
operator|->
name|nsize
argument_list|)
operator|)
condition|)
block|{
return|return
name|NULL
return|;
block|}
name|ancur
operator|->
name|icurr
operator|=
literal|0
expr_stmt|;
name|ancur
operator|->
name|next
operator|=
name|NULL
expr_stmt|;
if|if
condition|(
name|cha
operator|->
name|tail
condition|)
name|cha
operator|->
name|tail
operator|->
name|next
operator|=
name|ancur
expr_stmt|;
if|if
condition|(
operator|!
name|cha
operator|->
name|head
condition|)
name|cha
operator|->
name|head
operator|=
name|ancur
expr_stmt|;
name|cha
operator|->
name|tail
operator|=
name|ancur
expr_stmt|;
name|cha
operator|->
name|ancur
operator|=
name|ancur
expr_stmt|;
block|}
name|data
operator|=
operator|(
name|char
operator|*
operator|)
name|ancur
operator|+
sizeof|sizeof
argument_list|(
name|chanode_t
argument_list|)
operator|+
name|ancur
operator|->
name|icurr
expr_stmt|;
name|ancur
operator|->
name|icurr
operator|+=
name|cha
operator|->
name|isize
expr_stmt|;
return|return
name|data
return|;
block|}
end_function
begin_function
DECL|function|xdl_guess_lines
name|long
name|xdl_guess_lines
parameter_list|(
name|mmfile_t
modifier|*
name|mf
parameter_list|,
name|long
name|sample
parameter_list|)
block|{
name|long
name|nl
init|=
literal|0
decl_stmt|,
name|size
decl_stmt|,
name|tsize
init|=
literal|0
decl_stmt|;
name|char
specifier|const
modifier|*
name|data
decl_stmt|,
modifier|*
name|cur
decl_stmt|,
modifier|*
name|top
decl_stmt|;
if|if
condition|(
operator|(
name|cur
operator|=
name|data
operator|=
name|xdl_mmfile_first
argument_list|(
name|mf
argument_list|,
operator|&
name|size
argument_list|)
operator|)
operator|!=
name|NULL
condition|)
block|{
for|for
control|(
name|top
operator|=
name|data
operator|+
name|size
init|;
name|nl
operator|<
name|sample
operator|&&
name|cur
operator|<
name|top
condition|;
control|)
block|{
name|nl
operator|++
expr_stmt|;
if|if
condition|(
operator|!
operator|(
name|cur
operator|=
name|memchr
argument_list|(
name|cur
argument_list|,
literal|'\n'
argument_list|,
name|top
operator|-
name|cur
argument_list|)
operator|)
condition|)
name|cur
operator|=
name|top
expr_stmt|;
else|else
name|cur
operator|++
expr_stmt|;
block|}
name|tsize
operator|+=
call|(
name|long
call|)
argument_list|(
name|cur
operator|-
name|data
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|nl
operator|&&
name|tsize
condition|)
name|nl
operator|=
name|xdl_mmfile_size
argument_list|(
name|mf
argument_list|)
operator|/
operator|(
name|tsize
operator|/
name|nl
operator|)
expr_stmt|;
return|return
name|nl
operator|+
literal|1
return|;
block|}
end_function
begin_function
DECL|function|xdl_blankline
name|int
name|xdl_blankline
parameter_list|(
specifier|const
name|char
modifier|*
name|line
parameter_list|,
name|long
name|size
parameter_list|,
name|long
name|flags
parameter_list|)
block|{
name|long
name|i
decl_stmt|;
if|if
condition|(
operator|!
operator|(
name|flags
operator|&
name|XDF_WHITESPACE_FLAGS
operator|)
condition|)
return|return
operator|(
name|size
operator|<=
literal|1
operator|)
return|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|size
operator|&&
name|XDL_ISSPACE
argument_list|(
name|line
index|[
name|i
index|]
argument_list|)
condition|;
name|i
operator|++
control|)
empty_stmt|;
return|return
operator|(
name|i
operator|==
name|size
operator|)
return|;
block|}
end_function
begin_function
DECL|function|xdl_recmatch
name|int
name|xdl_recmatch
parameter_list|(
specifier|const
name|char
modifier|*
name|l1
parameter_list|,
name|long
name|s1
parameter_list|,
specifier|const
name|char
modifier|*
name|l2
parameter_list|,
name|long
name|s2
parameter_list|,
name|long
name|flags
parameter_list|)
block|{
name|int
name|i1
decl_stmt|,
name|i2
decl_stmt|;
if|if
condition|(
name|s1
operator|==
name|s2
operator|&&
operator|!
name|memcmp
argument_list|(
name|l1
argument_list|,
name|l2
argument_list|,
name|s1
argument_list|)
condition|)
return|return
literal|1
return|;
if|if
condition|(
operator|!
operator|(
name|flags
operator|&
name|XDF_WHITESPACE_FLAGS
operator|)
condition|)
return|return
literal|0
return|;
name|i1
operator|=
literal|0
expr_stmt|;
name|i2
operator|=
literal|0
expr_stmt|;
comment|/* 	 * -w matches everything that matches with -b, and -b in turn 	 * matches everything that matches with --ignore-space-at-eol. 	 * 	 * Each flavor of ignoring needs different logic to skip whitespaces 	 * while we have both sides to compare. 	 */
if|if
condition|(
name|flags
operator|&
name|XDF_IGNORE_WHITESPACE
condition|)
block|{
goto|goto
name|skip_ws
goto|;
while|while
condition|(
name|i1
operator|<
name|s1
operator|&&
name|i2
operator|<
name|s2
condition|)
block|{
if|if
condition|(
name|l1
index|[
name|i1
operator|++
index|]
operator|!=
name|l2
index|[
name|i2
operator|++
index|]
condition|)
return|return
literal|0
return|;
name|skip_ws
label|:
while|while
condition|(
name|i1
operator|<
name|s1
operator|&&
name|XDL_ISSPACE
argument_list|(
name|l1
index|[
name|i1
index|]
argument_list|)
condition|)
name|i1
operator|++
expr_stmt|;
while|while
condition|(
name|i2
operator|<
name|s2
operator|&&
name|XDL_ISSPACE
argument_list|(
name|l2
index|[
name|i2
index|]
argument_list|)
condition|)
name|i2
operator|++
expr_stmt|;
block|}
block|}
elseif|else
if|if
condition|(
name|flags
operator|&
name|XDF_IGNORE_WHITESPACE_CHANGE
condition|)
block|{
while|while
condition|(
name|i1
operator|<
name|s1
operator|&&
name|i2
operator|<
name|s2
condition|)
block|{
if|if
condition|(
name|XDL_ISSPACE
argument_list|(
name|l1
index|[
name|i1
index|]
argument_list|)
operator|&&
name|XDL_ISSPACE
argument_list|(
name|l2
index|[
name|i2
index|]
argument_list|)
condition|)
block|{
comment|/* Skip matching spaces and try again */
while|while
condition|(
name|i1
operator|<
name|s1
operator|&&
name|XDL_ISSPACE
argument_list|(
name|l1
index|[
name|i1
index|]
argument_list|)
condition|)
name|i1
operator|++
expr_stmt|;
while|while
condition|(
name|i2
operator|<
name|s2
operator|&&
name|XDL_ISSPACE
argument_list|(
name|l2
index|[
name|i2
index|]
argument_list|)
condition|)
name|i2
operator|++
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
name|l1
index|[
name|i1
operator|++
index|]
operator|!=
name|l2
index|[
name|i2
operator|++
index|]
condition|)
return|return
literal|0
return|;
block|}
block|}
elseif|else
if|if
condition|(
name|flags
operator|&
name|XDF_IGNORE_WHITESPACE_AT_EOL
condition|)
block|{
while|while
condition|(
name|i1
operator|<
name|s1
operator|&&
name|i2
operator|<
name|s2
operator|&&
name|l1
index|[
name|i1
operator|++
index|]
operator|==
name|l2
index|[
name|i2
operator|++
index|]
condition|)
empty_stmt|;
comment|/* keep going */
block|}
comment|/* 	 * After running out of one side, the remaining side must have 	 * nothing but whitespace for the lines to match.  Note that 	 * ignore-whitespace-at-eol case may break out of the loop 	 * while there still are characters remaining on both lines. 	 */
if|if
condition|(
name|i1
operator|<
name|s1
condition|)
block|{
while|while
condition|(
name|i1
operator|<
name|s1
operator|&&
name|XDL_ISSPACE
argument_list|(
name|l1
index|[
name|i1
index|]
argument_list|)
condition|)
name|i1
operator|++
expr_stmt|;
if|if
condition|(
name|s1
operator|!=
name|i1
condition|)
return|return
literal|0
return|;
block|}
if|if
condition|(
name|i2
operator|<
name|s2
condition|)
block|{
while|while
condition|(
name|i2
operator|<
name|s2
operator|&&
name|XDL_ISSPACE
argument_list|(
name|l2
index|[
name|i2
index|]
argument_list|)
condition|)
name|i2
operator|++
expr_stmt|;
return|return
operator|(
name|s2
operator|==
name|i2
operator|)
return|;
block|}
return|return
literal|1
return|;
block|}
end_function
begin_function
DECL|function|xdl_hash_record_with_whitespace
specifier|static
name|unsigned
name|long
name|xdl_hash_record_with_whitespace
parameter_list|(
name|char
specifier|const
modifier|*
modifier|*
name|data
parameter_list|,
name|char
specifier|const
modifier|*
name|top
parameter_list|,
name|long
name|flags
parameter_list|)
block|{
name|unsigned
name|long
name|ha
init|=
literal|5381
decl_stmt|;
name|char
specifier|const
modifier|*
name|ptr
init|=
operator|*
name|data
decl_stmt|;
for|for
control|(
init|;
name|ptr
operator|<
name|top
operator|&&
operator|*
name|ptr
operator|!=
literal|'\n'
condition|;
name|ptr
operator|++
control|)
block|{
if|if
condition|(
name|XDL_ISSPACE
argument_list|(
operator|*
name|ptr
argument_list|)
condition|)
block|{
specifier|const
name|char
modifier|*
name|ptr2
init|=
name|ptr
decl_stmt|;
name|int
name|at_eol
decl_stmt|;
while|while
condition|(
name|ptr
operator|+
literal|1
operator|<
name|top
operator|&&
name|XDL_ISSPACE
argument_list|(
name|ptr
index|[
literal|1
index|]
argument_list|)
operator|&&
name|ptr
index|[
literal|1
index|]
operator|!=
literal|'\n'
condition|)
name|ptr
operator|++
expr_stmt|;
name|at_eol
operator|=
operator|(
name|top
operator|<=
name|ptr
operator|+
literal|1
operator|||
name|ptr
index|[
literal|1
index|]
operator|==
literal|'\n'
operator|)
expr_stmt|;
if|if
condition|(
name|flags
operator|&
name|XDF_IGNORE_WHITESPACE
condition|)
empty_stmt|;
comment|/* already handled */
elseif|else
if|if
condition|(
name|flags
operator|&
name|XDF_IGNORE_WHITESPACE_CHANGE
operator|&&
operator|!
name|at_eol
condition|)
block|{
name|ha
operator|+=
operator|(
name|ha
operator|<<
literal|5
operator|)
expr_stmt|;
name|ha
operator|^=
operator|(
name|unsigned
name|long
operator|)
literal|' '
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|flags
operator|&
name|XDF_IGNORE_WHITESPACE_AT_EOL
operator|&&
operator|!
name|at_eol
condition|)
block|{
while|while
condition|(
name|ptr2
operator|!=
name|ptr
operator|+
literal|1
condition|)
block|{
name|ha
operator|+=
operator|(
name|ha
operator|<<
literal|5
operator|)
expr_stmt|;
name|ha
operator|^=
operator|(
name|unsigned
name|long
operator|)
operator|*
name|ptr2
expr_stmt|;
name|ptr2
operator|++
expr_stmt|;
block|}
block|}
continue|continue;
block|}
name|ha
operator|+=
operator|(
name|ha
operator|<<
literal|5
operator|)
expr_stmt|;
name|ha
operator|^=
operator|(
name|unsigned
name|long
operator|)
operator|*
name|ptr
expr_stmt|;
block|}
operator|*
name|data
operator|=
name|ptr
operator|<
name|top
condition|?
name|ptr
operator|+
literal|1
else|:
name|ptr
expr_stmt|;
return|return
name|ha
return|;
block|}
end_function
begin_ifdef
ifdef|#
directive|ifdef
name|XDL_FAST_HASH
end_ifdef
begin_define
DECL|macro|REPEAT_BYTE
define|#
directive|define
name|REPEAT_BYTE
parameter_list|(
name|x
parameter_list|)
value|((~0ul / 0xff) * (x))
end_define
begin_define
DECL|macro|ONEBYTES
define|#
directive|define
name|ONEBYTES
value|REPEAT_BYTE(0x01)
end_define
begin_define
DECL|macro|NEWLINEBYTES
define|#
directive|define
name|NEWLINEBYTES
value|REPEAT_BYTE(0x0a)
end_define
begin_define
DECL|macro|HIGHBITS
define|#
directive|define
name|HIGHBITS
value|REPEAT_BYTE(0x80)
end_define
begin_comment
comment|/* Return the high bit set in the first byte that is a zero */
end_comment
begin_function
DECL|function|has_zero
specifier|static
specifier|inline
name|unsigned
name|long
name|has_zero
parameter_list|(
name|unsigned
name|long
name|a
parameter_list|)
block|{
return|return
operator|(
operator|(
name|a
operator|-
name|ONEBYTES
operator|)
operator|&
operator|~
name|a
operator|)
operator|&
name|HIGHBITS
return|;
block|}
end_function
begin_function
DECL|function|count_masked_bytes
specifier|static
specifier|inline
name|long
name|count_masked_bytes
parameter_list|(
name|unsigned
name|long
name|mask
parameter_list|)
block|{
if|if
condition|(
sizeof|sizeof
argument_list|(
name|long
argument_list|)
operator|==
literal|8
condition|)
block|{
comment|/* 		 * Jan Achrenius on G+: microoptimized version of 		 * the simpler "(mask& ONEBYTES) * ONEBYTES>> 56" 		 * that works for the bytemasks without having to 		 * mask them first. 		 */
comment|/* 		 * return mask * 0x0001020304050608>> 56; 		 * 		 * Doing it like this avoids warnings on 32-bit machines. 		 */
name|long
name|a
init|=
operator|(
name|REPEAT_BYTE
argument_list|(
literal|0x01
argument_list|)
operator|/
literal|0xff
operator|+
literal|1
operator|)
decl_stmt|;
return|return
name|mask
operator|*
name|a
operator|>>
operator|(
sizeof|sizeof
argument_list|(
name|long
argument_list|)
operator|*
literal|7
operator|)
return|;
block|}
else|else
block|{
comment|/* Carl Chatfield / Jan Achrenius G+ version for 32-bit */
comment|/* (000000 0000ff 00ffff ffffff) -> ( 1 1 2 3 ) */
name|long
name|a
init|=
operator|(
literal|0x0ff0001
operator|+
name|mask
operator|)
operator|>>
literal|23
decl_stmt|;
comment|/* Fix the 1 for 00 case */
return|return
name|a
operator|&
name|mask
return|;
block|}
block|}
end_function
begin_function
DECL|function|xdl_hash_record
name|unsigned
name|long
name|xdl_hash_record
parameter_list|(
name|char
specifier|const
modifier|*
modifier|*
name|data
parameter_list|,
name|char
specifier|const
modifier|*
name|top
parameter_list|,
name|long
name|flags
parameter_list|)
block|{
name|unsigned
name|long
name|hash
init|=
literal|5381
decl_stmt|;
name|unsigned
name|long
name|a
init|=
literal|0
decl_stmt|,
name|mask
init|=
literal|0
decl_stmt|;
name|char
specifier|const
modifier|*
name|ptr
init|=
operator|*
name|data
decl_stmt|;
name|char
specifier|const
modifier|*
name|end
init|=
name|top
operator|-
expr|sizeof
operator|(
name|unsigned
name|long
operator|)
operator|+
literal|1
decl_stmt|;
if|if
condition|(
name|flags
operator|&
name|XDF_WHITESPACE_FLAGS
condition|)
return|return
name|xdl_hash_record_with_whitespace
argument_list|(
name|data
argument_list|,
name|top
argument_list|,
name|flags
argument_list|)
return|;
name|ptr
operator|-=
expr|sizeof
operator|(
name|unsigned
name|long
operator|)
expr_stmt|;
do|do
block|{
name|hash
operator|+=
name|hash
operator|<<
literal|5
expr_stmt|;
name|hash
operator|^=
name|a
expr_stmt|;
name|ptr
operator|+=
expr|sizeof
operator|(
name|unsigned
name|long
operator|)
expr_stmt|;
if|if
condition|(
name|ptr
operator|>=
name|end
condition|)
break|break;
name|a
operator|=
operator|*
operator|(
name|unsigned
name|long
operator|*
operator|)
name|ptr
expr_stmt|;
comment|/* Do we have any '\n' bytes in this word? */
name|mask
operator|=
name|has_zero
argument_list|(
name|a
operator|^
name|NEWLINEBYTES
argument_list|)
expr_stmt|;
block|}
do|while
condition|(
operator|!
name|mask
condition|)
do|;
if|if
condition|(
name|ptr
operator|>=
name|end
condition|)
block|{
comment|/* 		 * There is only a partial word left at the end of the 		 * buffer. Because we may work with a memory mapping, 		 * we have to grab the rest byte by byte instead of 		 * blindly reading it. 		 * 		 * To avoid problems with masking in a signed value, 		 * we use an unsigned char here. 		 */
specifier|const
name|char
modifier|*
name|p
decl_stmt|;
for|for
control|(
name|p
operator|=
name|top
operator|-
literal|1
init|;
name|p
operator|>=
name|ptr
condition|;
name|p
operator|--
control|)
name|a
operator|=
operator|(
name|a
operator|<<
literal|8
operator|)
operator|+
operator|*
operator|(
operator|(
specifier|const
name|unsigned
name|char
operator|*
operator|)
name|p
operator|)
expr_stmt|;
name|mask
operator|=
name|has_zero
argument_list|(
name|a
operator|^
name|NEWLINEBYTES
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|mask
condition|)
comment|/* 			 * No '\n' found in the partial word.  Make a 			 * mask that matches what we read. 			 */
name|mask
operator|=
literal|1UL
operator|<<
operator|(
literal|8
operator|*
operator|(
name|top
operator|-
name|ptr
operator|)
operator|+
literal|7
operator|)
expr_stmt|;
block|}
comment|/* The mask *below* the first high bit set */
name|mask
operator|=
operator|(
name|mask
operator|-
literal|1
operator|)
operator|&
operator|~
name|mask
expr_stmt|;
name|mask
operator|>>=
literal|7
expr_stmt|;
name|hash
operator|+=
name|hash
operator|<<
literal|5
expr_stmt|;
name|hash
operator|^=
name|a
operator|&
name|mask
expr_stmt|;
comment|/* Advance past the last (possibly partial) word */
name|ptr
operator|+=
name|count_masked_bytes
argument_list|(
name|mask
argument_list|)
expr_stmt|;
if|if
condition|(
name|ptr
operator|<
name|top
condition|)
block|{
name|assert
argument_list|(
operator|*
name|ptr
operator|==
literal|'\n'
argument_list|)
expr_stmt|;
name|ptr
operator|++
expr_stmt|;
block|}
operator|*
name|data
operator|=
name|ptr
expr_stmt|;
return|return
name|hash
return|;
block|}
end_function
begin_else
else|#
directive|else
end_else
begin_comment
comment|/* XDL_FAST_HASH */
end_comment
begin_function
DECL|function|xdl_hash_record
name|unsigned
name|long
name|xdl_hash_record
parameter_list|(
name|char
specifier|const
modifier|*
modifier|*
name|data
parameter_list|,
name|char
specifier|const
modifier|*
name|top
parameter_list|,
name|long
name|flags
parameter_list|)
block|{
name|unsigned
name|long
name|ha
init|=
literal|5381
decl_stmt|;
name|char
specifier|const
modifier|*
name|ptr
init|=
operator|*
name|data
decl_stmt|;
if|if
condition|(
name|flags
operator|&
name|XDF_WHITESPACE_FLAGS
condition|)
return|return
name|xdl_hash_record_with_whitespace
argument_list|(
name|data
argument_list|,
name|top
argument_list|,
name|flags
argument_list|)
return|;
for|for
control|(
init|;
name|ptr
operator|<
name|top
operator|&&
operator|*
name|ptr
operator|!=
literal|'\n'
condition|;
name|ptr
operator|++
control|)
block|{
name|ha
operator|+=
operator|(
name|ha
operator|<<
literal|5
operator|)
expr_stmt|;
name|ha
operator|^=
operator|(
name|unsigned
name|long
operator|)
operator|*
name|ptr
expr_stmt|;
block|}
operator|*
name|data
operator|=
name|ptr
operator|<
name|top
condition|?
name|ptr
operator|+
literal|1
else|:
name|ptr
expr_stmt|;
return|return
name|ha
return|;
block|}
end_function
begin_endif
endif|#
directive|endif
end_endif
begin_comment
comment|/* XDL_FAST_HASH */
end_comment
begin_function
DECL|function|xdl_hashbits
name|unsigned
name|int
name|xdl_hashbits
parameter_list|(
name|unsigned
name|int
name|size
parameter_list|)
block|{
name|unsigned
name|int
name|val
init|=
literal|1
decl_stmt|,
name|bits
init|=
literal|0
decl_stmt|;
for|for
control|(
init|;
name|val
operator|<
name|size
operator|&&
name|bits
operator|<
name|CHAR_BIT
operator|*
expr|sizeof
operator|(
name|unsigned
name|int
operator|)
condition|;
name|val
operator|<<=
literal|1
operator|,
name|bits
operator|++
control|)
empty_stmt|;
return|return
name|bits
condition|?
name|bits
else|:
literal|1
return|;
block|}
end_function
begin_function
DECL|function|xdl_num_out
name|int
name|xdl_num_out
parameter_list|(
name|char
modifier|*
name|out
parameter_list|,
name|long
name|val
parameter_list|)
block|{
name|char
modifier|*
name|ptr
decl_stmt|,
modifier|*
name|str
init|=
name|out
decl_stmt|;
name|char
name|buf
index|[
literal|32
index|]
decl_stmt|;
name|ptr
operator|=
name|buf
operator|+
sizeof|sizeof
argument_list|(
name|buf
argument_list|)
operator|-
literal|1
expr_stmt|;
operator|*
name|ptr
operator|=
literal|'\0'
expr_stmt|;
if|if
condition|(
name|val
operator|<
literal|0
condition|)
block|{
operator|*
operator|--
name|ptr
operator|=
literal|'-'
expr_stmt|;
name|val
operator|=
operator|-
name|val
expr_stmt|;
block|}
for|for
control|(
init|;
name|val
operator|&&
name|ptr
operator|>
name|buf
condition|;
name|val
operator|/=
literal|10
control|)
operator|*
operator|--
name|ptr
operator|=
literal|"0123456789"
index|[
name|val
operator|%
literal|10
index|]
expr_stmt|;
if|if
condition|(
operator|*
name|ptr
condition|)
for|for
control|(
init|;
operator|*
name|ptr
condition|;
name|ptr
operator|++
operator|,
name|str
operator|++
control|)
operator|*
name|str
operator|=
operator|*
name|ptr
expr_stmt|;
else|else
operator|*
name|str
operator|++
operator|=
literal|'0'
expr_stmt|;
operator|*
name|str
operator|=
literal|'\0'
expr_stmt|;
return|return
name|str
operator|-
name|out
return|;
block|}
end_function
begin_function
DECL|function|xdl_emit_hunk_hdr
name|int
name|xdl_emit_hunk_hdr
parameter_list|(
name|long
name|s1
parameter_list|,
name|long
name|c1
parameter_list|,
name|long
name|s2
parameter_list|,
name|long
name|c2
parameter_list|,
specifier|const
name|char
modifier|*
name|func
parameter_list|,
name|long
name|funclen
parameter_list|,
name|xdemitcb_t
modifier|*
name|ecb
parameter_list|)
block|{
name|int
name|nb
init|=
literal|0
decl_stmt|;
name|mmbuffer_t
name|mb
decl_stmt|;
name|char
name|buf
index|[
literal|128
index|]
decl_stmt|;
name|memcpy
argument_list|(
name|buf
argument_list|,
literal|"@@ -"
argument_list|,
literal|4
argument_list|)
expr_stmt|;
name|nb
operator|+=
literal|4
expr_stmt|;
name|nb
operator|+=
name|xdl_num_out
argument_list|(
name|buf
operator|+
name|nb
argument_list|,
name|c1
condition|?
name|s1
else|:
name|s1
operator|-
literal|1
argument_list|)
expr_stmt|;
if|if
condition|(
name|c1
operator|!=
literal|1
condition|)
block|{
name|memcpy
argument_list|(
name|buf
operator|+
name|nb
argument_list|,
literal|","
argument_list|,
literal|1
argument_list|)
expr_stmt|;
name|nb
operator|+=
literal|1
expr_stmt|;
name|nb
operator|+=
name|xdl_num_out
argument_list|(
name|buf
operator|+
name|nb
argument_list|,
name|c1
argument_list|)
expr_stmt|;
block|}
name|memcpy
argument_list|(
name|buf
operator|+
name|nb
argument_list|,
literal|" +"
argument_list|,
literal|2
argument_list|)
expr_stmt|;
name|nb
operator|+=
literal|2
expr_stmt|;
name|nb
operator|+=
name|xdl_num_out
argument_list|(
name|buf
operator|+
name|nb
argument_list|,
name|c2
condition|?
name|s2
else|:
name|s2
operator|-
literal|1
argument_list|)
expr_stmt|;
if|if
condition|(
name|c2
operator|!=
literal|1
condition|)
block|{
name|memcpy
argument_list|(
name|buf
operator|+
name|nb
argument_list|,
literal|","
argument_list|,
literal|1
argument_list|)
expr_stmt|;
name|nb
operator|+=
literal|1
expr_stmt|;
name|nb
operator|+=
name|xdl_num_out
argument_list|(
name|buf
operator|+
name|nb
argument_list|,
name|c2
argument_list|)
expr_stmt|;
block|}
name|memcpy
argument_list|(
name|buf
operator|+
name|nb
argument_list|,
literal|" @@"
argument_list|,
literal|3
argument_list|)
expr_stmt|;
name|nb
operator|+=
literal|3
expr_stmt|;
if|if
condition|(
name|func
operator|&&
name|funclen
condition|)
block|{
name|buf
index|[
name|nb
operator|++
index|]
operator|=
literal|' '
expr_stmt|;
if|if
condition|(
name|funclen
operator|>
sizeof|sizeof
argument_list|(
name|buf
argument_list|)
operator|-
name|nb
operator|-
literal|1
condition|)
name|funclen
operator|=
sizeof|sizeof
argument_list|(
name|buf
argument_list|)
operator|-
name|nb
operator|-
literal|1
expr_stmt|;
name|memcpy
argument_list|(
name|buf
operator|+
name|nb
argument_list|,
name|func
argument_list|,
name|funclen
argument_list|)
expr_stmt|;
name|nb
operator|+=
name|funclen
expr_stmt|;
block|}
name|buf
index|[
name|nb
operator|++
index|]
operator|=
literal|'\n'
expr_stmt|;
name|mb
operator|.
name|ptr
operator|=
name|buf
expr_stmt|;
name|mb
operator|.
name|size
operator|=
name|nb
expr_stmt|;
if|if
condition|(
name|ecb
operator|->
name|outf
argument_list|(
name|ecb
operator|->
name|priv
argument_list|,
operator|&
name|mb
argument_list|,
literal|1
argument_list|)
operator|<
literal|0
condition|)
return|return
operator|-
literal|1
return|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|xdl_fall_back_diff
name|int
name|xdl_fall_back_diff
parameter_list|(
name|xdfenv_t
modifier|*
name|diff_env
parameter_list|,
name|xpparam_t
specifier|const
modifier|*
name|xpp
parameter_list|,
name|int
name|line1
parameter_list|,
name|int
name|count1
parameter_list|,
name|int
name|line2
parameter_list|,
name|int
name|count2
parameter_list|)
block|{
comment|/* 	 * This probably does not work outside Git, since 	 * we have a very simple mmfile structure. 	 * 	 * Note: ideally, we would reuse the prepared environment, but 	 * the libxdiff interface does not (yet) allow for diffing only 	 * ranges of lines instead of the whole files. 	 */
name|mmfile_t
name|subfile1
decl_stmt|,
name|subfile2
decl_stmt|;
name|xdfenv_t
name|env
decl_stmt|;
name|subfile1
operator|.
name|ptr
operator|=
operator|(
name|char
operator|*
operator|)
name|diff_env
operator|->
name|xdf1
operator|.
name|recs
index|[
name|line1
operator|-
literal|1
index|]
operator|->
name|ptr
expr_stmt|;
name|subfile1
operator|.
name|size
operator|=
name|diff_env
operator|->
name|xdf1
operator|.
name|recs
index|[
name|line1
operator|+
name|count1
operator|-
literal|2
index|]
operator|->
name|ptr
operator|+
name|diff_env
operator|->
name|xdf1
operator|.
name|recs
index|[
name|line1
operator|+
name|count1
operator|-
literal|2
index|]
operator|->
name|size
operator|-
name|subfile1
operator|.
name|ptr
expr_stmt|;
name|subfile2
operator|.
name|ptr
operator|=
operator|(
name|char
operator|*
operator|)
name|diff_env
operator|->
name|xdf2
operator|.
name|recs
index|[
name|line2
operator|-
literal|1
index|]
operator|->
name|ptr
expr_stmt|;
name|subfile2
operator|.
name|size
operator|=
name|diff_env
operator|->
name|xdf2
operator|.
name|recs
index|[
name|line2
operator|+
name|count2
operator|-
literal|2
index|]
operator|->
name|ptr
operator|+
name|diff_env
operator|->
name|xdf2
operator|.
name|recs
index|[
name|line2
operator|+
name|count2
operator|-
literal|2
index|]
operator|->
name|size
operator|-
name|subfile2
operator|.
name|ptr
expr_stmt|;
if|if
condition|(
name|xdl_do_diff
argument_list|(
operator|&
name|subfile1
argument_list|,
operator|&
name|subfile2
argument_list|,
name|xpp
argument_list|,
operator|&
name|env
argument_list|)
operator|<
literal|0
condition|)
return|return
operator|-
literal|1
return|;
name|memcpy
argument_list|(
name|diff_env
operator|->
name|xdf1
operator|.
name|rchg
operator|+
name|line1
operator|-
literal|1
argument_list|,
name|env
operator|.
name|xdf1
operator|.
name|rchg
argument_list|,
name|count1
argument_list|)
expr_stmt|;
name|memcpy
argument_list|(
name|diff_env
operator|->
name|xdf2
operator|.
name|rchg
operator|+
name|line2
operator|-
literal|1
argument_list|,
name|env
operator|.
name|xdf2
operator|.
name|rchg
argument_list|,
name|count2
argument_list|)
expr_stmt|;
name|xdl_free_env
argument_list|(
operator|&
name|env
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
end_unit
