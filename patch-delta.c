begin_unit
begin_comment
comment|/*  * patch-delta.c:  * recreate a buffer from a source and the delta produced by diff-delta.c  *  * (C) 2005 Nicolas Pitre<nico@fluxnic.net>  *  * This code is free software; you can redistribute it and/or modify  * it under the terms of the GNU General Public License version 2 as  * published by the Free Software Foundation.  */
end_comment
begin_include
include|#
directive|include
file|"git-compat-util.h"
end_include
begin_include
include|#
directive|include
file|"delta.h"
end_include
begin_function
DECL|function|patch_delta
name|void
modifier|*
name|patch_delta
parameter_list|(
specifier|const
name|void
modifier|*
name|src_buf
parameter_list|,
name|unsigned
name|long
name|src_size
parameter_list|,
specifier|const
name|void
modifier|*
name|delta_buf
parameter_list|,
name|unsigned
name|long
name|delta_size
parameter_list|,
name|unsigned
name|long
modifier|*
name|dst_size
parameter_list|)
block|{
specifier|const
name|unsigned
name|char
modifier|*
name|data
decl_stmt|,
modifier|*
name|top
decl_stmt|;
name|unsigned
name|char
modifier|*
name|dst_buf
decl_stmt|,
modifier|*
name|out
decl_stmt|,
name|cmd
decl_stmt|;
name|unsigned
name|long
name|size
decl_stmt|;
if|if
condition|(
name|delta_size
operator|<
name|DELTA_SIZE_MIN
condition|)
return|return
name|NULL
return|;
name|data
operator|=
name|delta_buf
expr_stmt|;
name|top
operator|=
operator|(
specifier|const
name|unsigned
name|char
operator|*
operator|)
name|delta_buf
operator|+
name|delta_size
expr_stmt|;
comment|/* make sure the orig file size matches what we expect */
name|size
operator|=
name|get_delta_hdr_size
argument_list|(
operator|&
name|data
argument_list|,
name|top
argument_list|)
expr_stmt|;
if|if
condition|(
name|size
operator|!=
name|src_size
condition|)
return|return
name|NULL
return|;
comment|/* now the result size */
name|size
operator|=
name|get_delta_hdr_size
argument_list|(
operator|&
name|data
argument_list|,
name|top
argument_list|)
expr_stmt|;
name|dst_buf
operator|=
name|xmallocz
argument_list|(
name|size
argument_list|)
expr_stmt|;
name|out
operator|=
name|dst_buf
expr_stmt|;
while|while
condition|(
name|data
operator|<
name|top
condition|)
block|{
name|cmd
operator|=
operator|*
name|data
operator|++
expr_stmt|;
if|if
condition|(
name|cmd
operator|&
literal|0x80
condition|)
block|{
name|unsigned
name|long
name|cp_off
init|=
literal|0
decl_stmt|,
name|cp_size
init|=
literal|0
decl_stmt|;
if|if
condition|(
name|cmd
operator|&
literal|0x01
condition|)
name|cp_off
operator|=
operator|*
name|data
operator|++
expr_stmt|;
if|if
condition|(
name|cmd
operator|&
literal|0x02
condition|)
name|cp_off
operator||=
operator|(
operator|*
name|data
operator|++
operator|<<
literal|8
operator|)
expr_stmt|;
if|if
condition|(
name|cmd
operator|&
literal|0x04
condition|)
name|cp_off
operator||=
operator|(
operator|*
name|data
operator|++
operator|<<
literal|16
operator|)
expr_stmt|;
if|if
condition|(
name|cmd
operator|&
literal|0x08
condition|)
name|cp_off
operator||=
operator|(
operator|(
name|unsigned
operator|)
operator|*
name|data
operator|++
operator|<<
literal|24
operator|)
expr_stmt|;
if|if
condition|(
name|cmd
operator|&
literal|0x10
condition|)
name|cp_size
operator|=
operator|*
name|data
operator|++
expr_stmt|;
if|if
condition|(
name|cmd
operator|&
literal|0x20
condition|)
name|cp_size
operator||=
operator|(
operator|*
name|data
operator|++
operator|<<
literal|8
operator|)
expr_stmt|;
if|if
condition|(
name|cmd
operator|&
literal|0x40
condition|)
name|cp_size
operator||=
operator|(
operator|*
name|data
operator|++
operator|<<
literal|16
operator|)
expr_stmt|;
if|if
condition|(
name|cp_size
operator|==
literal|0
condition|)
name|cp_size
operator|=
literal|0x10000
expr_stmt|;
if|if
condition|(
name|cp_off
operator|+
name|cp_size
operator|<
name|cp_size
operator|||
name|cp_off
operator|+
name|cp_size
operator|>
name|src_size
operator|||
name|cp_size
operator|>
name|size
condition|)
break|break;
name|memcpy
argument_list|(
name|out
argument_list|,
operator|(
name|char
operator|*
operator|)
name|src_buf
operator|+
name|cp_off
argument_list|,
name|cp_size
argument_list|)
expr_stmt|;
name|out
operator|+=
name|cp_size
expr_stmt|;
name|size
operator|-=
name|cp_size
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|cmd
condition|)
block|{
if|if
condition|(
name|cmd
operator|>
name|size
condition|)
break|break;
name|memcpy
argument_list|(
name|out
argument_list|,
name|data
argument_list|,
name|cmd
argument_list|)
expr_stmt|;
name|out
operator|+=
name|cmd
expr_stmt|;
name|data
operator|+=
name|cmd
expr_stmt|;
name|size
operator|-=
name|cmd
expr_stmt|;
block|}
else|else
block|{
comment|/* 			 * cmd == 0 is reserved for future encoding 			 * extensions. In the mean time we must fail when 			 * encountering them (might be data corruption). 			 */
name|error
argument_list|(
literal|"unexpected delta opcode 0"
argument_list|)
expr_stmt|;
goto|goto
name|bad
goto|;
block|}
block|}
comment|/* sanity check */
if|if
condition|(
name|data
operator|!=
name|top
operator|||
name|size
operator|!=
literal|0
condition|)
block|{
name|error
argument_list|(
literal|"delta replay has gone wild"
argument_list|)
expr_stmt|;
name|bad
label|:
name|free
argument_list|(
name|dst_buf
argument_list|)
expr_stmt|;
return|return
name|NULL
return|;
block|}
operator|*
name|dst_size
operator|=
name|out
operator|-
name|dst_buf
expr_stmt|;
return|return
name|dst_buf
return|;
block|}
end_function
end_unit
