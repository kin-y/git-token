begin_unit
begin_comment
comment|/* **  Do shell-style pattern matching for ?, \, [], and * characters. **  It is 8bit clean. ** **  Written by Rich $alz, mirror!rs, Wed Nov 26 19:03:17 EST 1986. **  Rich $alz is now<rsalz@bbn.com>. ** **  Modified by Wayne Davison to special-case '/' matching, to make '**' **  work differently than '*', and to fix the character-class code. */
end_comment
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"wildmatch.h"
end_include
begin_typedef
DECL|typedef|uchar
typedef|typedef
name|unsigned
name|char
name|uchar
typedef|;
end_typedef
begin_comment
comment|/* What character marks an inverted character class? */
end_comment
begin_define
DECL|macro|NEGATE_CLASS
define|#
directive|define
name|NEGATE_CLASS
value|'!'
end_define
begin_define
DECL|macro|NEGATE_CLASS2
define|#
directive|define
name|NEGATE_CLASS2
value|'^'
end_define
begin_define
DECL|macro|CC_EQ
define|#
directive|define
name|CC_EQ
parameter_list|(
name|class
parameter_list|,
name|len
parameter_list|,
name|litmatch
parameter_list|)
value|((len) == sizeof (litmatch)-1 \&& *(class) == *(litmatch) \&& strncmp((char*)class, litmatch, len) == 0)
end_define
begin_if
if|#
directive|if
name|defined
name|STDC_HEADERS
operator|||
operator|!
name|defined
name|isascii
end_if
begin_define
DECL|macro|ISASCII
define|#
directive|define
name|ISASCII
parameter_list|(
name|c
parameter_list|)
value|1
end_define
begin_else
else|#
directive|else
end_else
begin_define
DECL|macro|ISASCII
define|#
directive|define
name|ISASCII
parameter_list|(
name|c
parameter_list|)
value|isascii(c)
end_define
begin_endif
endif|#
directive|endif
end_endif
begin_ifdef
ifdef|#
directive|ifdef
name|isblank
end_ifdef
begin_define
DECL|macro|ISBLANK
define|#
directive|define
name|ISBLANK
parameter_list|(
name|c
parameter_list|)
value|(ISASCII(c)&& isblank(c))
end_define
begin_else
else|#
directive|else
end_else
begin_define
DECL|macro|ISBLANK
define|#
directive|define
name|ISBLANK
parameter_list|(
name|c
parameter_list|)
value|((c) == ' ' || (c) == '\t')
end_define
begin_endif
endif|#
directive|endif
end_endif
begin_ifdef
ifdef|#
directive|ifdef
name|isgraph
end_ifdef
begin_define
DECL|macro|ISGRAPH
define|#
directive|define
name|ISGRAPH
parameter_list|(
name|c
parameter_list|)
value|(ISASCII(c)&& isgraph(c))
end_define
begin_else
else|#
directive|else
end_else
begin_define
DECL|macro|ISGRAPH
define|#
directive|define
name|ISGRAPH
parameter_list|(
name|c
parameter_list|)
value|(ISASCII(c)&& isprint(c)&& !isspace(c))
end_define
begin_endif
endif|#
directive|endif
end_endif
begin_define
DECL|macro|ISPRINT
define|#
directive|define
name|ISPRINT
parameter_list|(
name|c
parameter_list|)
value|(ISASCII(c)&& isprint(c))
end_define
begin_define
DECL|macro|ISDIGIT
define|#
directive|define
name|ISDIGIT
parameter_list|(
name|c
parameter_list|)
value|(ISASCII(c)&& isdigit(c))
end_define
begin_define
DECL|macro|ISALNUM
define|#
directive|define
name|ISALNUM
parameter_list|(
name|c
parameter_list|)
value|(ISASCII(c)&& isalnum(c))
end_define
begin_define
DECL|macro|ISALPHA
define|#
directive|define
name|ISALPHA
parameter_list|(
name|c
parameter_list|)
value|(ISASCII(c)&& isalpha(c))
end_define
begin_define
DECL|macro|ISCNTRL
define|#
directive|define
name|ISCNTRL
parameter_list|(
name|c
parameter_list|)
value|(ISASCII(c)&& iscntrl(c))
end_define
begin_define
DECL|macro|ISLOWER
define|#
directive|define
name|ISLOWER
parameter_list|(
name|c
parameter_list|)
value|(ISASCII(c)&& islower(c))
end_define
begin_define
DECL|macro|ISPUNCT
define|#
directive|define
name|ISPUNCT
parameter_list|(
name|c
parameter_list|)
value|(ISASCII(c)&& ispunct(c))
end_define
begin_define
DECL|macro|ISSPACE
define|#
directive|define
name|ISSPACE
parameter_list|(
name|c
parameter_list|)
value|(ISASCII(c)&& isspace(c))
end_define
begin_define
DECL|macro|ISUPPER
define|#
directive|define
name|ISUPPER
parameter_list|(
name|c
parameter_list|)
value|(ISASCII(c)&& isupper(c))
end_define
begin_define
DECL|macro|ISXDIGIT
define|#
directive|define
name|ISXDIGIT
parameter_list|(
name|c
parameter_list|)
value|(ISASCII(c)&& isxdigit(c))
end_define
begin_comment
comment|/* Match pattern "p" against "text" */
end_comment
begin_function
DECL|function|dowild
specifier|static
name|int
name|dowild
parameter_list|(
specifier|const
name|uchar
modifier|*
name|p
parameter_list|,
specifier|const
name|uchar
modifier|*
name|text
parameter_list|,
name|unsigned
name|int
name|flags
parameter_list|)
block|{
name|uchar
name|p_ch
decl_stmt|;
specifier|const
name|uchar
modifier|*
name|pattern
init|=
name|p
decl_stmt|;
for|for
control|(
init|;
operator|(
name|p_ch
operator|=
operator|*
name|p
operator|)
operator|!=
literal|'\0'
condition|;
name|text
operator|++
operator|,
name|p
operator|++
control|)
block|{
name|int
name|matched
decl_stmt|,
name|match_slash
decl_stmt|,
name|negated
decl_stmt|;
name|uchar
name|t_ch
decl_stmt|,
name|prev_ch
decl_stmt|;
if|if
condition|(
operator|(
name|t_ch
operator|=
operator|*
name|text
operator|)
operator|==
literal|'\0'
operator|&&
name|p_ch
operator|!=
literal|'*'
condition|)
return|return
name|WM_ABORT_ALL
return|;
if|if
condition|(
operator|(
name|flags
operator|&
name|WM_CASEFOLD
operator|)
operator|&&
name|ISUPPER
argument_list|(
name|t_ch
argument_list|)
condition|)
name|t_ch
operator|=
name|tolower
argument_list|(
name|t_ch
argument_list|)
expr_stmt|;
if|if
condition|(
operator|(
name|flags
operator|&
name|WM_CASEFOLD
operator|)
operator|&&
name|ISUPPER
argument_list|(
name|p_ch
argument_list|)
condition|)
name|p_ch
operator|=
name|tolower
argument_list|(
name|p_ch
argument_list|)
expr_stmt|;
switch|switch
condition|(
name|p_ch
condition|)
block|{
case|case
literal|'\\'
case|:
comment|/* Literal match with following character.  Note that the test 			 * in "default" handles the p[1] == '\0' failure case. */
name|p_ch
operator|=
operator|*
operator|++
name|p
expr_stmt|;
comment|/* FALLTHROUGH */
default|default:
if|if
condition|(
name|t_ch
operator|!=
name|p_ch
condition|)
return|return
name|WM_NOMATCH
return|;
continue|continue;
case|case
literal|'?'
case|:
comment|/* Match anything but '/'. */
if|if
condition|(
operator|(
name|flags
operator|&
name|WM_PATHNAME
operator|)
operator|&&
name|t_ch
operator|==
literal|'/'
condition|)
return|return
name|WM_NOMATCH
return|;
continue|continue;
case|case
literal|'*'
case|:
if|if
condition|(
operator|*
operator|++
name|p
operator|==
literal|'*'
condition|)
block|{
specifier|const
name|uchar
modifier|*
name|prev_p
init|=
name|p
operator|-
literal|2
decl_stmt|;
while|while
condition|(
operator|*
operator|++
name|p
operator|==
literal|'*'
condition|)
block|{}
if|if
condition|(
operator|!
operator|(
name|flags
operator|&
name|WM_PATHNAME
operator|)
condition|)
comment|/* without WM_PATHNAME, '*' == '**' */
name|match_slash
operator|=
literal|1
expr_stmt|;
elseif|else
if|if
condition|(
operator|(
name|prev_p
operator|<
name|pattern
operator|||
operator|*
name|prev_p
operator|==
literal|'/'
operator|)
operator|&&
operator|(
operator|*
name|p
operator|==
literal|'\0'
operator|||
operator|*
name|p
operator|==
literal|'/'
operator|||
operator|(
name|p
index|[
literal|0
index|]
operator|==
literal|'\\'
operator|&&
name|p
index|[
literal|1
index|]
operator|==
literal|'/'
operator|)
operator|)
condition|)
block|{
comment|/* 					 * Assuming we already match 'foo/' and are at 					 *<star star slash>, just assume it matches 					 * nothing and go ahead match the rest of the 					 * pattern with the remaining string. This 					 * helps make foo/<*><*>/bar (<> because 					 * otherwise it breaks C comment syntax) match 					 * both foo/bar and foo/a/bar. 					 */
if|if
condition|(
name|p
index|[
literal|0
index|]
operator|==
literal|'/'
operator|&&
name|dowild
argument_list|(
name|p
operator|+
literal|1
argument_list|,
name|text
argument_list|,
name|flags
argument_list|)
operator|==
name|WM_MATCH
condition|)
return|return
name|WM_MATCH
return|;
name|match_slash
operator|=
literal|1
expr_stmt|;
block|}
else|else
return|return
name|WM_ABORT_MALFORMED
return|;
block|}
else|else
comment|/* without WM_PATHNAME, '*' == '**' */
name|match_slash
operator|=
name|flags
operator|&
name|WM_PATHNAME
condition|?
literal|0
else|:
literal|1
expr_stmt|;
if|if
condition|(
operator|*
name|p
operator|==
literal|'\0'
condition|)
block|{
comment|/* Trailing "**" matches everything.  Trailing "*" matches 				 * only if there are no more slash characters. */
if|if
condition|(
operator|!
name|match_slash
condition|)
block|{
if|if
condition|(
name|strchr
argument_list|(
operator|(
name|char
operator|*
operator|)
name|text
argument_list|,
literal|'/'
argument_list|)
operator|!=
name|NULL
condition|)
return|return
name|WM_NOMATCH
return|;
block|}
return|return
name|WM_MATCH
return|;
block|}
elseif|else
if|if
condition|(
operator|!
name|match_slash
operator|&&
operator|*
name|p
operator|==
literal|'/'
condition|)
block|{
comment|/* 				 * _one_ asterisk followed by a slash 				 * with WM_PATHNAME matches the next 				 * directory 				 */
specifier|const
name|char
modifier|*
name|slash
init|=
name|strchr
argument_list|(
operator|(
name|char
operator|*
operator|)
name|text
argument_list|,
literal|'/'
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
name|slash
condition|)
return|return
name|WM_NOMATCH
return|;
name|text
operator|=
operator|(
specifier|const
name|uchar
operator|*
operator|)
name|slash
expr_stmt|;
comment|/* the slash is consumed by the top-level for loop */
break|break;
block|}
while|while
condition|(
literal|1
condition|)
block|{
if|if
condition|(
name|t_ch
operator|==
literal|'\0'
condition|)
break|break;
comment|/* 				 * Try to advance faster when an asterisk is 				 * followed by a literal. We know in this case 				 * that the the string before the literal 				 * must belong to "*". 				 * If match_slash is false, do not look past 				 * the first slash as it cannot belong to '*'. 				 */
if|if
condition|(
operator|!
name|is_glob_special
argument_list|(
operator|*
name|p
argument_list|)
condition|)
block|{
name|p_ch
operator|=
operator|*
name|p
expr_stmt|;
if|if
condition|(
operator|(
name|flags
operator|&
name|WM_CASEFOLD
operator|)
operator|&&
name|ISUPPER
argument_list|(
name|p_ch
argument_list|)
condition|)
name|p_ch
operator|=
name|tolower
argument_list|(
name|p_ch
argument_list|)
expr_stmt|;
while|while
condition|(
operator|(
name|t_ch
operator|=
operator|*
name|text
operator|)
operator|!=
literal|'\0'
operator|&&
operator|(
name|match_slash
operator|||
name|t_ch
operator|!=
literal|'/'
operator|)
condition|)
block|{
if|if
condition|(
operator|(
name|flags
operator|&
name|WM_CASEFOLD
operator|)
operator|&&
name|ISUPPER
argument_list|(
name|t_ch
argument_list|)
condition|)
name|t_ch
operator|=
name|tolower
argument_list|(
name|t_ch
argument_list|)
expr_stmt|;
if|if
condition|(
name|t_ch
operator|==
name|p_ch
condition|)
break|break;
name|text
operator|++
expr_stmt|;
block|}
if|if
condition|(
name|t_ch
operator|!=
name|p_ch
condition|)
return|return
name|WM_NOMATCH
return|;
block|}
if|if
condition|(
operator|(
name|matched
operator|=
name|dowild
argument_list|(
name|p
argument_list|,
name|text
argument_list|,
name|flags
argument_list|)
operator|)
operator|!=
name|WM_NOMATCH
condition|)
block|{
if|if
condition|(
operator|!
name|match_slash
operator|||
name|matched
operator|!=
name|WM_ABORT_TO_STARSTAR
condition|)
return|return
name|matched
return|;
block|}
elseif|else
if|if
condition|(
operator|!
name|match_slash
operator|&&
name|t_ch
operator|==
literal|'/'
condition|)
return|return
name|WM_ABORT_TO_STARSTAR
return|;
name|t_ch
operator|=
operator|*
operator|++
name|text
expr_stmt|;
block|}
return|return
name|WM_ABORT_ALL
return|;
case|case
literal|'['
case|:
name|p_ch
operator|=
operator|*
operator|++
name|p
expr_stmt|;
ifdef|#
directive|ifdef
name|NEGATE_CLASS2
if|if
condition|(
name|p_ch
operator|==
name|NEGATE_CLASS2
condition|)
name|p_ch
operator|=
name|NEGATE_CLASS
expr_stmt|;
endif|#
directive|endif
comment|/* Assign literal 1/0 because of "matched" comparison. */
name|negated
operator|=
name|p_ch
operator|==
name|NEGATE_CLASS
condition|?
literal|1
else|:
literal|0
expr_stmt|;
if|if
condition|(
name|negated
condition|)
block|{
comment|/* Inverted character class. */
name|p_ch
operator|=
operator|*
operator|++
name|p
expr_stmt|;
block|}
name|prev_ch
operator|=
literal|0
expr_stmt|;
name|matched
operator|=
literal|0
expr_stmt|;
do|do
block|{
if|if
condition|(
operator|!
name|p_ch
condition|)
return|return
name|WM_ABORT_ALL
return|;
if|if
condition|(
name|p_ch
operator|==
literal|'\\'
condition|)
block|{
name|p_ch
operator|=
operator|*
operator|++
name|p
expr_stmt|;
if|if
condition|(
operator|!
name|p_ch
condition|)
return|return
name|WM_ABORT_ALL
return|;
if|if
condition|(
name|t_ch
operator|==
name|p_ch
condition|)
name|matched
operator|=
literal|1
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|p_ch
operator|==
literal|'-'
operator|&&
name|prev_ch
operator|&&
name|p
index|[
literal|1
index|]
operator|&&
name|p
index|[
literal|1
index|]
operator|!=
literal|']'
condition|)
block|{
name|p_ch
operator|=
operator|*
operator|++
name|p
expr_stmt|;
if|if
condition|(
name|p_ch
operator|==
literal|'\\'
condition|)
block|{
name|p_ch
operator|=
operator|*
operator|++
name|p
expr_stmt|;
if|if
condition|(
operator|!
name|p_ch
condition|)
return|return
name|WM_ABORT_ALL
return|;
block|}
if|if
condition|(
name|t_ch
operator|<=
name|p_ch
operator|&&
name|t_ch
operator|>=
name|prev_ch
condition|)
name|matched
operator|=
literal|1
expr_stmt|;
elseif|else
if|if
condition|(
operator|(
name|flags
operator|&
name|WM_CASEFOLD
operator|)
operator|&&
name|ISLOWER
argument_list|(
name|t_ch
argument_list|)
condition|)
block|{
name|uchar
name|t_ch_upper
init|=
name|toupper
argument_list|(
name|t_ch
argument_list|)
decl_stmt|;
if|if
condition|(
name|t_ch_upper
operator|<=
name|p_ch
operator|&&
name|t_ch_upper
operator|>=
name|prev_ch
condition|)
name|matched
operator|=
literal|1
expr_stmt|;
block|}
name|p_ch
operator|=
literal|0
expr_stmt|;
comment|/* This makes "prev_ch" get set to 0. */
block|}
elseif|else
if|if
condition|(
name|p_ch
operator|==
literal|'['
operator|&&
name|p
index|[
literal|1
index|]
operator|==
literal|':'
condition|)
block|{
specifier|const
name|uchar
modifier|*
name|s
decl_stmt|;
name|int
name|i
decl_stmt|;
for|for
control|(
name|s
operator|=
name|p
operator|+=
literal|2
init|;
operator|(
name|p_ch
operator|=
operator|*
name|p
operator|)
operator|&&
name|p_ch
operator|!=
literal|']'
condition|;
name|p
operator|++
control|)
block|{}
comment|/*SHARED ITERATOR*/
if|if
condition|(
operator|!
name|p_ch
condition|)
return|return
name|WM_ABORT_ALL
return|;
name|i
operator|=
name|p
operator|-
name|s
operator|-
literal|1
expr_stmt|;
if|if
condition|(
name|i
operator|<
literal|0
operator|||
name|p
index|[
operator|-
literal|1
index|]
operator|!=
literal|':'
condition|)
block|{
comment|/* Didn't find ":]", so treat like a normal set. */
name|p
operator|=
name|s
operator|-
literal|2
expr_stmt|;
name|p_ch
operator|=
literal|'['
expr_stmt|;
if|if
condition|(
name|t_ch
operator|==
name|p_ch
condition|)
name|matched
operator|=
literal|1
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
name|CC_EQ
argument_list|(
name|s
argument_list|,
name|i
argument_list|,
literal|"alnum"
argument_list|)
condition|)
block|{
if|if
condition|(
name|ISALNUM
argument_list|(
name|t_ch
argument_list|)
condition|)
name|matched
operator|=
literal|1
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|CC_EQ
argument_list|(
name|s
argument_list|,
name|i
argument_list|,
literal|"alpha"
argument_list|)
condition|)
block|{
if|if
condition|(
name|ISALPHA
argument_list|(
name|t_ch
argument_list|)
condition|)
name|matched
operator|=
literal|1
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|CC_EQ
argument_list|(
name|s
argument_list|,
name|i
argument_list|,
literal|"blank"
argument_list|)
condition|)
block|{
if|if
condition|(
name|ISBLANK
argument_list|(
name|t_ch
argument_list|)
condition|)
name|matched
operator|=
literal|1
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|CC_EQ
argument_list|(
name|s
argument_list|,
name|i
argument_list|,
literal|"cntrl"
argument_list|)
condition|)
block|{
if|if
condition|(
name|ISCNTRL
argument_list|(
name|t_ch
argument_list|)
condition|)
name|matched
operator|=
literal|1
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|CC_EQ
argument_list|(
name|s
argument_list|,
name|i
argument_list|,
literal|"digit"
argument_list|)
condition|)
block|{
if|if
condition|(
name|ISDIGIT
argument_list|(
name|t_ch
argument_list|)
condition|)
name|matched
operator|=
literal|1
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|CC_EQ
argument_list|(
name|s
argument_list|,
name|i
argument_list|,
literal|"graph"
argument_list|)
condition|)
block|{
if|if
condition|(
name|ISGRAPH
argument_list|(
name|t_ch
argument_list|)
condition|)
name|matched
operator|=
literal|1
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|CC_EQ
argument_list|(
name|s
argument_list|,
name|i
argument_list|,
literal|"lower"
argument_list|)
condition|)
block|{
if|if
condition|(
name|ISLOWER
argument_list|(
name|t_ch
argument_list|)
condition|)
name|matched
operator|=
literal|1
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|CC_EQ
argument_list|(
name|s
argument_list|,
name|i
argument_list|,
literal|"print"
argument_list|)
condition|)
block|{
if|if
condition|(
name|ISPRINT
argument_list|(
name|t_ch
argument_list|)
condition|)
name|matched
operator|=
literal|1
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|CC_EQ
argument_list|(
name|s
argument_list|,
name|i
argument_list|,
literal|"punct"
argument_list|)
condition|)
block|{
if|if
condition|(
name|ISPUNCT
argument_list|(
name|t_ch
argument_list|)
condition|)
name|matched
operator|=
literal|1
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|CC_EQ
argument_list|(
name|s
argument_list|,
name|i
argument_list|,
literal|"space"
argument_list|)
condition|)
block|{
if|if
condition|(
name|ISSPACE
argument_list|(
name|t_ch
argument_list|)
condition|)
name|matched
operator|=
literal|1
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|CC_EQ
argument_list|(
name|s
argument_list|,
name|i
argument_list|,
literal|"upper"
argument_list|)
condition|)
block|{
if|if
condition|(
name|ISUPPER
argument_list|(
name|t_ch
argument_list|)
condition|)
name|matched
operator|=
literal|1
expr_stmt|;
elseif|else
if|if
condition|(
operator|(
name|flags
operator|&
name|WM_CASEFOLD
operator|)
operator|&&
name|ISLOWER
argument_list|(
name|t_ch
argument_list|)
condition|)
name|matched
operator|=
literal|1
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|CC_EQ
argument_list|(
name|s
argument_list|,
name|i
argument_list|,
literal|"xdigit"
argument_list|)
condition|)
block|{
if|if
condition|(
name|ISXDIGIT
argument_list|(
name|t_ch
argument_list|)
condition|)
name|matched
operator|=
literal|1
expr_stmt|;
block|}
else|else
comment|/* malformed [:class:] string */
return|return
name|WM_ABORT_ALL
return|;
name|p_ch
operator|=
literal|0
expr_stmt|;
comment|/* This makes "prev_ch" get set to 0. */
block|}
elseif|else
if|if
condition|(
name|t_ch
operator|==
name|p_ch
condition|)
name|matched
operator|=
literal|1
expr_stmt|;
block|}
do|while
condition|(
name|prev_ch
operator|=
name|p_ch
operator|,
operator|(
name|p_ch
operator|=
operator|*
operator|++
name|p
operator|)
operator|!=
literal|']'
condition|)
do|;
if|if
condition|(
name|matched
operator|==
name|negated
operator|||
operator|(
operator|(
name|flags
operator|&
name|WM_PATHNAME
operator|)
operator|&&
name|t_ch
operator|==
literal|'/'
operator|)
condition|)
return|return
name|WM_NOMATCH
return|;
continue|continue;
block|}
block|}
return|return
operator|*
name|text
condition|?
name|WM_NOMATCH
else|:
name|WM_MATCH
return|;
block|}
end_function
begin_comment
comment|/* Match the "pattern" against the "text" string. */
end_comment
begin_function
DECL|function|wildmatch
name|int
name|wildmatch
parameter_list|(
specifier|const
name|char
modifier|*
name|pattern
parameter_list|,
specifier|const
name|char
modifier|*
name|text
parameter_list|,
name|unsigned
name|int
name|flags
parameter_list|,
name|struct
name|wildopts
modifier|*
name|wo
parameter_list|)
block|{
return|return
name|dowild
argument_list|(
operator|(
specifier|const
name|uchar
operator|*
operator|)
name|pattern
argument_list|,
operator|(
specifier|const
name|uchar
operator|*
operator|)
name|text
argument_list|,
name|flags
argument_list|)
return|;
block|}
end_function
end_unit
