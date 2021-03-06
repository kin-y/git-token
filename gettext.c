begin_unit
begin_comment
comment|/*  * Copyright (c) 2010 Ãvar ArnfjÃ¶rÃ° Bjarmason  */
end_comment
begin_include
include|#
directive|include
file|"git-compat-util.h"
end_include
begin_include
include|#
directive|include
file|"gettext.h"
end_include
begin_include
include|#
directive|include
file|"strbuf.h"
end_include
begin_include
include|#
directive|include
file|"utf8.h"
end_include
begin_ifndef
ifndef|#
directive|ifndef
name|NO_GETTEXT
end_ifndef
begin_include
include|#
directive|include
file|<locale.h>
end_include
begin_include
include|#
directive|include
file|<libintl.h>
end_include
begin_ifdef
ifdef|#
directive|ifdef
name|HAVE_LIBCHARSET_H
end_ifdef
begin_include
include|#
directive|include
file|<libcharset.h>
end_include
begin_else
else|#
directive|else
end_else
begin_include
include|#
directive|include
file|<langinfo.h>
end_include
begin_define
DECL|macro|locale_charset
define|#
directive|define
name|locale_charset
parameter_list|()
value|nl_langinfo(CODESET)
end_define
begin_endif
endif|#
directive|endif
end_endif
begin_endif
endif|#
directive|endif
end_endif
begin_comment
comment|/*  * Guess the user's preferred languages from the value in LANGUAGE environment  * variable and LC_MESSAGES locale category if NO_GETTEXT is not defined.  *  * The result can be a colon-separated list like "ko:ja:en".  */
end_comment
begin_function
DECL|function|get_preferred_languages
specifier|const
name|char
modifier|*
name|get_preferred_languages
parameter_list|(
name|void
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|retval
decl_stmt|;
name|retval
operator|=
name|getenv
argument_list|(
literal|"LANGUAGE"
argument_list|)
expr_stmt|;
if|if
condition|(
name|retval
operator|&&
operator|*
name|retval
condition|)
return|return
name|retval
return|;
ifndef|#
directive|ifndef
name|NO_GETTEXT
name|retval
operator|=
name|setlocale
argument_list|(
name|LC_MESSAGES
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
if|if
condition|(
name|retval
operator|&&
operator|*
name|retval
operator|&&
name|strcmp
argument_list|(
name|retval
argument_list|,
literal|"C"
argument_list|)
operator|&&
name|strcmp
argument_list|(
name|retval
argument_list|,
literal|"POSIX"
argument_list|)
condition|)
return|return
name|retval
return|;
endif|#
directive|endif
return|return
name|NULL
return|;
block|}
end_function
begin_ifdef
ifdef|#
directive|ifdef
name|GETTEXT_POISON
end_ifdef
begin_function
DECL|function|use_gettext_poison
name|int
name|use_gettext_poison
parameter_list|(
name|void
parameter_list|)
block|{
specifier|static
name|int
name|poison_requested
init|=
operator|-
literal|1
decl_stmt|;
if|if
condition|(
name|poison_requested
operator|==
operator|-
literal|1
condition|)
name|poison_requested
operator|=
name|getenv
argument_list|(
literal|"GIT_GETTEXT_POISON"
argument_list|)
condition|?
literal|1
else|:
literal|0
expr_stmt|;
return|return
name|poison_requested
return|;
block|}
end_function
begin_endif
endif|#
directive|endif
end_endif
begin_ifndef
ifndef|#
directive|ifndef
name|NO_GETTEXT
end_ifndef
begin_function
DECL|function|test_vsnprintf
specifier|static
name|int
name|test_vsnprintf
parameter_list|(
specifier|const
name|char
modifier|*
name|fmt
parameter_list|,
modifier|...
parameter_list|)
block|{
name|char
name|buf
index|[
literal|26
index|]
decl_stmt|;
name|int
name|ret
decl_stmt|;
name|va_list
name|ap
decl_stmt|;
name|va_start
argument_list|(
name|ap
argument_list|,
name|fmt
argument_list|)
expr_stmt|;
name|ret
operator|=
name|vsnprintf
argument_list|(
name|buf
argument_list|,
sizeof|sizeof
argument_list|(
name|buf
argument_list|)
argument_list|,
name|fmt
argument_list|,
name|ap
argument_list|)
expr_stmt|;
name|va_end
argument_list|(
name|ap
argument_list|)
expr_stmt|;
return|return
name|ret
return|;
block|}
end_function
begin_decl_stmt
DECL|variable|charset
specifier|static
specifier|const
name|char
modifier|*
name|charset
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|init_gettext_charset
specifier|static
name|void
name|init_gettext_charset
parameter_list|(
specifier|const
name|char
modifier|*
name|domain
parameter_list|)
block|{
comment|/* 	   This trick arranges for messages to be emitted in the user's 	   requested encoding, but avoids setting LC_CTYPE from the 	   environment for the whole program.  	   This primarily done to avoid a bug in vsnprintf in the GNU C 	   Library [1]. which triggered a "your vsnprintf is broken" error 	   on Git's own repository when inspecting v0.99.6~1 under a UTF-8 	   locale.  	   That commit contains a ISO-8859-1 encoded author name, which 	   the locale aware vsnprintf(3) won't interpolate in the format 	   argument, due to mismatch between the data encoding and the 	   locale.  	   Even if it wasn't for that bug we wouldn't want to use LC_CTYPE at 	   this point, because it'd require auditing all the code that uses C 	   functions whose semantics are modified by LC_CTYPE.  	   But only setting LC_MESSAGES as we do creates a problem, since 	   we declare the encoding of our PO files[2] the gettext 	   implementation will try to recode it to the user's locale, but 	   without LC_CTYPE it'll emit something like this on 'git init' 	   under the Icelandic locale:  	       Bj? til t?ma Git lind ? /hlagh/.git/  	   Gettext knows about the encoding of our PO file, but we haven't 	   told it about the user's encoding, so all the non-US-ASCII 	   characters get encoded to question marks.  	   But we're in luck! We can set LC_CTYPE from the environment 	   only while we call nl_langinfo and 	   bind_textdomain_codeset. That suffices to tell gettext what 	   encoding it should emit in, so it'll now say:  	       BjÃ³ til tÃ³ma Git lind Ã­ /hlagh/.git/  	   And the equivalent ISO-8859-1 string will be emitted under a 	   ISO-8859-1 locale.  	   With this change way we get the advantages of setting LC_CTYPE 	   (talk to the user in his language/encoding), without the major 	   drawbacks (changed semantics for C functions we rely on).  	   However foreign functions using other message catalogs that 	   aren't using our neat trick will still have a problem, e.g. if 	   we have to call perror(3):  	   #include<stdio.h> 	   #include<locale.h> 	   #include<errno.h>  	   int main(void) 	   { 		   setlocale(LC_MESSAGES, ""); 		   setlocale(LC_CTYPE, "C"); 		   errno = ENODEV; 		   perror("test"); 		   return 0; 	   }  	   Running that will give you a message with question marks:  	   $ LANGUAGE= LANG=de_DE.utf8 ./test 	   test: Kein passendes Ger?t gefunden  	   The vsnprintf bug has been fixed since glibc 2.17.  	   Then we could simply set LC_CTYPE from the environment, which would 	   make things like the external perror(3) messages work.  	   See t/t0203-gettext-setlocale-sanity.sh's "gettext.c" tests for 	   regression tests.  	   1. http://sourceware.org/bugzilla/show_bug.cgi?id=6530 	   2. E.g. "Content-Type: text/plain; charset=UTF-8\n" in po/is.po 	*/
name|setlocale
argument_list|(
name|LC_CTYPE
argument_list|,
literal|""
argument_list|)
expr_stmt|;
name|charset
operator|=
name|locale_charset
argument_list|()
expr_stmt|;
name|bind_textdomain_codeset
argument_list|(
name|domain
argument_list|,
name|charset
argument_list|)
expr_stmt|;
comment|/* the string is taken from v0.99.6~1 */
if|if
condition|(
name|test_vsnprintf
argument_list|(
literal|"%.*s"
argument_list|,
literal|13
argument_list|,
literal|"David_K\345gedal"
argument_list|)
operator|<
literal|0
condition|)
name|setlocale
argument_list|(
name|LC_CTYPE
argument_list|,
literal|"C"
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|git_setup_gettext
name|void
name|git_setup_gettext
parameter_list|(
name|void
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|podir
init|=
name|getenv
argument_list|(
literal|"GIT_TEXTDOMAINDIR"
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
name|podir
condition|)
name|podir
operator|=
name|GIT_LOCALE_PATH
expr_stmt|;
name|bindtextdomain
argument_list|(
literal|"git"
argument_list|,
name|podir
argument_list|)
expr_stmt|;
name|setlocale
argument_list|(
name|LC_MESSAGES
argument_list|,
literal|""
argument_list|)
expr_stmt|;
name|setlocale
argument_list|(
name|LC_TIME
argument_list|,
literal|""
argument_list|)
expr_stmt|;
name|init_gettext_charset
argument_list|(
literal|"git"
argument_list|)
expr_stmt|;
name|textdomain
argument_list|(
literal|"git"
argument_list|)
expr_stmt|;
block|}
end_function
begin_comment
comment|/* return the number of columns of string 's' in current locale */
end_comment
begin_function
DECL|function|gettext_width
name|int
name|gettext_width
parameter_list|(
specifier|const
name|char
modifier|*
name|s
parameter_list|)
block|{
specifier|static
name|int
name|is_utf8
init|=
operator|-
literal|1
decl_stmt|;
if|if
condition|(
name|is_utf8
operator|==
operator|-
literal|1
condition|)
name|is_utf8
operator|=
operator|!
name|strcmp
argument_list|(
name|charset
argument_list|,
literal|"UTF-8"
argument_list|)
expr_stmt|;
return|return
name|is_utf8
condition|?
name|utf8_strwidth
argument_list|(
name|s
argument_list|)
else|:
name|strlen
argument_list|(
name|s
argument_list|)
return|;
block|}
end_function
begin_endif
endif|#
directive|endif
end_endif
end_unit
