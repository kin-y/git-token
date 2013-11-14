begin_unit
begin_comment
comment|/**  * Copyright 2013, GitHub, Inc  * Copyright 2009-2013, Daniel Lemire, Cliff Moon,  *	David McIntosh, Robert Becho, Google Inc. and Veronika Zenz  *  * This program is free software; you can redistribute it and/or  * modify it under the terms of the GNU General Public License  * as published by the Free Software Foundation; either version 2  * of the License, or (at your option) any later version.  *  * This program is distributed in the hope that it will be useful,  * but WITHOUT ANY WARRANTY; without even the implied warranty of  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the  * GNU General Public License for more details.  *  * You should have received a copy of the GNU General Public License  * along with this program; if not, write to the Free Software  * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.  */
end_comment
begin_ifndef
ifndef|#
directive|ifndef
name|__EWOK_RLW_H__
end_ifndef
begin_define
DECL|macro|__EWOK_RLW_H__
define|#
directive|define
name|__EWOK_RLW_H__
end_define
begin_define
DECL|macro|RLW_RUNNING_BITS
define|#
directive|define
name|RLW_RUNNING_BITS
value|(sizeof(eword_t) * 4)
end_define
begin_define
DECL|macro|RLW_LITERAL_BITS
define|#
directive|define
name|RLW_LITERAL_BITS
value|(sizeof(eword_t) * 8 - 1 - RLW_RUNNING_BITS)
end_define
begin_define
DECL|macro|RLW_LARGEST_RUNNING_COUNT
define|#
directive|define
name|RLW_LARGEST_RUNNING_COUNT
value|(((eword_t)1<< RLW_RUNNING_BITS) - 1)
end_define
begin_define
DECL|macro|RLW_LARGEST_LITERAL_COUNT
define|#
directive|define
name|RLW_LARGEST_LITERAL_COUNT
value|(((eword_t)1<< RLW_LITERAL_BITS) - 1)
end_define
begin_define
DECL|macro|RLW_LARGEST_RUNNING_COUNT_SHIFT
define|#
directive|define
name|RLW_LARGEST_RUNNING_COUNT_SHIFT
value|(RLW_LARGEST_RUNNING_COUNT<< 1)
end_define
begin_define
DECL|macro|RLW_RUNNING_LEN_PLUS_BIT
define|#
directive|define
name|RLW_RUNNING_LEN_PLUS_BIT
value|(((eword_t)1<< (RLW_RUNNING_BITS + 1)) - 1)
end_define
begin_function
DECL|function|rlw_get_run_bit
specifier|static
name|int
name|rlw_get_run_bit
parameter_list|(
specifier|const
name|eword_t
modifier|*
name|word
parameter_list|)
block|{
return|return
operator|*
name|word
operator|&
operator|(
name|eword_t
operator|)
literal|1
return|;
block|}
end_function
begin_function
DECL|function|rlw_set_run_bit
specifier|static
specifier|inline
name|void
name|rlw_set_run_bit
parameter_list|(
name|eword_t
modifier|*
name|word
parameter_list|,
name|int
name|b
parameter_list|)
block|{
if|if
condition|(
name|b
condition|)
block|{
operator|*
name|word
operator||=
operator|(
name|eword_t
operator|)
literal|1
expr_stmt|;
block|}
else|else
block|{
operator|*
name|word
operator|&=
call|(
name|eword_t
call|)
argument_list|(
operator|~
literal|1
argument_list|)
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|rlw_xor_run_bit
specifier|static
specifier|inline
name|void
name|rlw_xor_run_bit
parameter_list|(
name|eword_t
modifier|*
name|word
parameter_list|)
block|{
if|if
condition|(
operator|*
name|word
operator|&
literal|1
condition|)
block|{
operator|*
name|word
operator|&=
call|(
name|eword_t
call|)
argument_list|(
operator|~
literal|1
argument_list|)
expr_stmt|;
block|}
else|else
block|{
operator|*
name|word
operator||=
operator|(
name|eword_t
operator|)
literal|1
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|rlw_set_running_len
specifier|static
specifier|inline
name|void
name|rlw_set_running_len
parameter_list|(
name|eword_t
modifier|*
name|word
parameter_list|,
name|eword_t
name|l
parameter_list|)
block|{
operator|*
name|word
operator||=
name|RLW_LARGEST_RUNNING_COUNT_SHIFT
expr_stmt|;
operator|*
name|word
operator|&=
operator|(
name|l
operator|<<
literal|1
operator|)
operator||
operator|(
operator|~
name|RLW_LARGEST_RUNNING_COUNT_SHIFT
operator|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|rlw_get_running_len
specifier|static
specifier|inline
name|eword_t
name|rlw_get_running_len
parameter_list|(
specifier|const
name|eword_t
modifier|*
name|word
parameter_list|)
block|{
return|return
operator|(
operator|*
name|word
operator|>>
literal|1
operator|)
operator|&
name|RLW_LARGEST_RUNNING_COUNT
return|;
block|}
end_function
begin_function
DECL|function|rlw_get_literal_words
specifier|static
specifier|inline
name|eword_t
name|rlw_get_literal_words
parameter_list|(
specifier|const
name|eword_t
modifier|*
name|word
parameter_list|)
block|{
return|return
operator|*
name|word
operator|>>
operator|(
literal|1
operator|+
name|RLW_RUNNING_BITS
operator|)
return|;
block|}
end_function
begin_function
DECL|function|rlw_set_literal_words
specifier|static
specifier|inline
name|void
name|rlw_set_literal_words
parameter_list|(
name|eword_t
modifier|*
name|word
parameter_list|,
name|eword_t
name|l
parameter_list|)
block|{
operator|*
name|word
operator||=
operator|~
name|RLW_RUNNING_LEN_PLUS_BIT
expr_stmt|;
operator|*
name|word
operator|&=
operator|(
name|l
operator|<<
operator|(
name|RLW_RUNNING_BITS
operator|+
literal|1
operator|)
operator|)
operator||
name|RLW_RUNNING_LEN_PLUS_BIT
expr_stmt|;
block|}
end_function
begin_function
DECL|function|rlw_size
specifier|static
specifier|inline
name|eword_t
name|rlw_size
parameter_list|(
specifier|const
name|eword_t
modifier|*
name|self
parameter_list|)
block|{
return|return
name|rlw_get_running_len
argument_list|(
name|self
argument_list|)
operator|+
name|rlw_get_literal_words
argument_list|(
name|self
argument_list|)
return|;
block|}
end_function
begin_struct
DECL|struct|rlw_iterator
struct|struct
name|rlw_iterator
block|{
DECL|member|buffer
specifier|const
name|eword_t
modifier|*
name|buffer
decl_stmt|;
DECL|member|size
name|size_t
name|size
decl_stmt|;
DECL|member|pointer
name|size_t
name|pointer
decl_stmt|;
DECL|member|literal_word_start
name|size_t
name|literal_word_start
decl_stmt|;
struct|struct
block|{
DECL|member|word
specifier|const
name|eword_t
modifier|*
name|word
decl_stmt|;
DECL|member|literal_words
name|int
name|literal_words
decl_stmt|;
DECL|member|running_len
name|int
name|running_len
decl_stmt|;
DECL|member|literal_word_offset
name|int
name|literal_word_offset
decl_stmt|;
DECL|member|running_bit
name|int
name|running_bit
decl_stmt|;
block|}
DECL|member|rlw
name|rlw
struct|;
block|}
struct|;
end_struct
begin_function_decl
name|void
name|rlwit_init
parameter_list|(
name|struct
name|rlw_iterator
modifier|*
name|it
parameter_list|,
name|struct
name|ewah_bitmap
modifier|*
name|bitmap
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|void
name|rlwit_discard_first_words
parameter_list|(
name|struct
name|rlw_iterator
modifier|*
name|it
parameter_list|,
name|size_t
name|x
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|size_t
name|rlwit_discharge
parameter_list|(
name|struct
name|rlw_iterator
modifier|*
name|it
parameter_list|,
name|struct
name|ewah_bitmap
modifier|*
name|out
parameter_list|,
name|size_t
name|max
parameter_list|,
name|int
name|negate
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|void
name|rlwit_discharge_empty
parameter_list|(
name|struct
name|rlw_iterator
modifier|*
name|it
parameter_list|,
name|struct
name|ewah_bitmap
modifier|*
name|out
parameter_list|)
function_decl|;
end_function_decl
begin_function
DECL|function|rlwit_word_size
specifier|static
specifier|inline
name|size_t
name|rlwit_word_size
parameter_list|(
name|struct
name|rlw_iterator
modifier|*
name|it
parameter_list|)
block|{
return|return
name|it
operator|->
name|rlw
operator|.
name|running_len
operator|+
name|it
operator|->
name|rlw
operator|.
name|literal_words
return|;
block|}
end_function
begin_function
DECL|function|rlwit_literal_words
specifier|static
specifier|inline
name|size_t
name|rlwit_literal_words
parameter_list|(
name|struct
name|rlw_iterator
modifier|*
name|it
parameter_list|)
block|{
return|return
name|it
operator|->
name|pointer
operator|-
name|it
operator|->
name|rlw
operator|.
name|literal_words
return|;
block|}
end_function
begin_endif
endif|#
directive|endif
end_endif
end_unit
