begin_unit
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"userdiff.h"
end_include
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
begin_decl_stmt
DECL|variable|drivers
specifier|static
name|struct
name|userdiff_driver
modifier|*
name|drivers
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|ndrivers
specifier|static
name|int
name|ndrivers
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|drivers_alloc
specifier|static
name|int
name|drivers_alloc
decl_stmt|;
end_decl_stmt
begin_define
DECL|macro|PATTERNS
define|#
directive|define
name|PATTERNS
parameter_list|(
name|name
parameter_list|,
name|pattern
parameter_list|,
name|word_regex
parameter_list|)
define|\
value|{ name, NULL, -1, { pattern, REG_EXTENDED },		\ 	  word_regex "|[^[:space:]]|[\xc0-\xff][\x80-\xbf]+" }
end_define
begin_define
DECL|macro|IPATTERN
define|#
directive|define
name|IPATTERN
parameter_list|(
name|name
parameter_list|,
name|pattern
parameter_list|,
name|word_regex
parameter_list|)
define|\
value|{ name, NULL, -1, { pattern, REG_EXTENDED | REG_ICASE }, \ 	  word_regex "|[^[:space:]]|[\xc0-\xff][\x80-\xbf]+" }
end_define
begin_decl_stmt
DECL|variable|builtin_drivers
specifier|static
name|struct
name|userdiff_driver
name|builtin_drivers
index|[]
init|=
block|{
name|IPATTERN
argument_list|(
literal|"fortran"
argument_list|,
literal|"!^([C*]|[ \t]*!)\n"
literal|"!^[ \t]*MODULE[ \t]+PROCEDURE[ \t]\n"
literal|"^[ \t]*((END[ \t]+)?(PROGRAM|MODULE|BLOCK[ \t]+DATA"
literal|"|([^'\" \t]+[ \t]+)*(SUBROUTINE|FUNCTION))[ \t]+[A-Z].*)$"
argument_list|,
comment|/* -- */
literal|"[a-zA-Z][a-zA-Z0-9_]*"
literal|"|\\.([Ee][Qq]|[Nn][Ee]|[Gg][TtEe]|[Ll][TtEe]|[Tt][Rr][Uu][Ee]|[Ff][Aa][Ll][Ss][Ee]|[Aa][Nn][Dd]|[Oo][Rr]|[Nn]?[Ee][Qq][Vv]|[Nn][Oo][Tt])\\."
comment|/* numbers and format statements like 2E14.4, or ES12.6, 9X. 	  * Don't worry about format statements without leading digits since 	  * they would have been matched above as a variable anyway. */
literal|"|[-+]?[0-9.]+([AaIiDdEeFfLlTtXx][Ss]?[-+]?[0-9.]*)?(_[a-zA-Z0-9][a-zA-Z0-9_]*)?"
literal|"|//|\\*\\*|::|[/<>=]="
argument_list|)
block|,
name|PATTERNS
argument_list|(
literal|"html"
argument_list|,
literal|"^[ \t]*(<[Hh][1-6][ \t].*>.*)$"
argument_list|,
literal|"[^<>= \t]+"
argument_list|)
block|,
name|PATTERNS
argument_list|(
literal|"java"
argument_list|,
literal|"!^[ \t]*(catch|do|for|if|instanceof|new|return|switch|throw|while)\n"
literal|"^[ \t]*(([A-Za-z_][A-Za-z_0-9]*[ \t]+)+[A-Za-z_][A-Za-z_0-9]*[ \t]*\\([^;]*)$"
argument_list|,
comment|/* -- */
literal|"[a-zA-Z_][a-zA-Z0-9_]*"
literal|"|[-+0-9.e]+[fFlL]?|0[xXbB]?[0-9a-fA-F]+[lL]?"
literal|"|[-+*/<>%&^|=!]="
literal|"|--|\\+\\+|<<=?|>>>?=?|&&|\\|\\|"
argument_list|)
block|,
name|PATTERNS
argument_list|(
literal|"objc"
argument_list|,
comment|/* Negate C statements that can look like functions */
literal|"!^[ \t]*(do|for|if|else|return|switch|while)\n"
comment|/* Objective-C methods */
literal|"^[ \t]*([-+][ \t]*\\([ \t]*[A-Za-z_][A-Za-z_0-9* \t]*\\)[ \t]*[A-Za-z_].*)$\n"
comment|/* C functions */
literal|"^[ \t]*(([A-Za-z_][A-Za-z_0-9]*[ \t]+)+[A-Za-z_][A-Za-z_0-9]*[ \t]*\\([^;]*)$\n"
comment|/* Objective-C class/protocol definitions */
literal|"^(@(implementation|interface|protocol)[ \t].*)$"
argument_list|,
comment|/* -- */
literal|"[a-zA-Z_][a-zA-Z0-9_]*"
literal|"|[-+0-9.e]+[fFlL]?|0[xXbB]?[0-9a-fA-F]+[lL]?"
literal|"|[-+*/<>%&^|=!]=|--|\\+\\+|<<=?|>>=?|&&|\\|\\||::|->"
argument_list|)
block|,
name|PATTERNS
argument_list|(
literal|"pascal"
argument_list|,
literal|"^(((class[ \t]+)?(procedure|function)|constructor|destructor|interface|"
literal|"implementation|initialization|finalization)[ \t]*.*)$"
literal|"\n"
literal|"^(.*=[ \t]*(class|record).*)$"
argument_list|,
comment|/* -- */
literal|"[a-zA-Z_][a-zA-Z0-9_]*"
literal|"|[-+0-9.e]+|0[xXbB]?[0-9a-fA-F]+"
literal|"|<>|<=|>=|:=|\\.\\."
argument_list|)
block|,
name|PATTERNS
argument_list|(
literal|"perl"
argument_list|,
literal|"^package .*\n"
literal|"^sub [[:alnum:]_':]+[ \t]*"
literal|"(\\([^)]*\\)[ \t]*)?"
comment|/* prototype */
comment|/* 		 * Attributes.  A regex can't count nested parentheses, 		 * so just slurp up whatever we see, taking care not 		 * to accept lines like "sub foo; # defined elsewhere". 		 * 		 * An attribute could contain a semicolon, but at that 		 * point it seems reasonable enough to give up. 		 */
literal|"(:[^;#]*)?"
literal|"(\\{[ \t]*)?"
comment|/* brace can come here or on the next line */
literal|"(#.*)?$\n"
comment|/* comment */
literal|"^(BEGIN|END|INIT|CHECK|UNITCHECK|AUTOLOAD|DESTROY)[ \t]*"
literal|"(\\{[ \t]*)?"
comment|/* brace can come here or on the next line */
literal|"(#.*)?$\n"
literal|"^=head[0-9] .*"
argument_list|,
comment|/* POD */
comment|/* -- */
literal|"[[:alpha:]_'][[:alnum:]_']*"
literal|"|0[xb]?[0-9a-fA-F_]*"
comment|/* taking care not to interpret 3..5 as (3.)(.5) */
literal|"|[0-9a-fA-F_]+(\\.[0-9a-fA-F_]+)?([eE][-+]?[0-9_]+)?"
literal|"|=>|-[rwxoRWXOezsfdlpSugkbctTBMAC>]|~~|::"
literal|"|&&=|\\|\\|=|//=|\\*\\*="
literal|"|&&|\\|\\||//|\\+\\+|--|\\*\\*|\\.\\.\\.?"
literal|"|[-+*/%.^&<>=!|]="
literal|"|=~|!~"
literal|"|<<|<>|<=>|>>"
argument_list|)
block|,
name|PATTERNS
argument_list|(
literal|"php"
argument_list|,
literal|"^[\t ]*(((public|protected|private|static)[\t ]+)*function.*)$\n"
literal|"^[\t ]*(class.*)$"
argument_list|,
comment|/* -- */
literal|"[a-zA-Z_][a-zA-Z0-9_]*"
literal|"|[-+0-9.e]+|0[xXbB]?[0-9a-fA-F]+"
literal|"|[-+*/<>%&^|=!.]=|--|\\+\\+|<<=?|>>=?|===|&&|\\|\\||::|->"
argument_list|)
block|,
name|PATTERNS
argument_list|(
literal|"python"
argument_list|,
literal|"^[ \t]*((class|def)[ \t].*)$"
argument_list|,
comment|/* -- */
literal|"[a-zA-Z_][a-zA-Z0-9_]*"
literal|"|[-+0-9.e]+[jJlL]?|0[xX]?[0-9a-fA-F]+[lL]?"
literal|"|[-+*/<>%&^|=!]=|//=?|<<=?|>>=?|\\*\\*=?"
argument_list|)
block|,
comment|/* -- */
name|PATTERNS
argument_list|(
literal|"ruby"
argument_list|,
literal|"^[ \t]*((class|module|def)[ \t].*)$"
argument_list|,
comment|/* -- */
literal|"(@|@@|\\$)?[a-zA-Z_][a-zA-Z0-9_]*"
literal|"|[-+0-9.e]+|0[xXbB]?[0-9a-fA-F]+|\\?(\\\\C-)?(\\\\M-)?."
literal|"|//=?|[-+*/<>%&^|=!]=|<<=?|>>=?|===|\\.{1,3}|::|[!=]~"
argument_list|)
block|,
name|PATTERNS
argument_list|(
literal|"bibtex"
argument_list|,
literal|"(@[a-zA-Z]{1,}[ \t]*\\{{0,1}[ \t]*[^ \t\"@',\\#}{~%]*).*$"
argument_list|,
literal|"[={}\"]|[^={}\" \t]+"
argument_list|)
block|,
name|PATTERNS
argument_list|(
literal|"tex"
argument_list|,
literal|"^(\\\\((sub)*section|chapter|part)\\*{0,1}\\{.*)$"
argument_list|,
literal|"\\\\[a-zA-Z@]+|\\\\.|[a-zA-Z0-9\x80-\xff]+"
argument_list|)
block|,
name|PATTERNS
argument_list|(
literal|"cpp"
argument_list|,
comment|/* Jump targets or access declarations */
literal|"!^[ \t]*[A-Za-z_][A-Za-z_0-9]*:.*$\n"
comment|/* C/++ functions/methods at top level */
literal|"^([A-Za-z_][A-Za-z_0-9]*([ \t*]+[A-Za-z_][A-Za-z_0-9]*([ \t]*::[ \t]*[^[:space:]]+)?){1,}[ \t]*\\([^;]*)$\n"
comment|/* compound type at top level */
literal|"^((struct|class|enum)[^;]*)$"
argument_list|,
comment|/* -- */
literal|"[a-zA-Z_][a-zA-Z0-9_]*"
literal|"|[-+0-9.e]+[fFlL]?|0[xXbB]?[0-9a-fA-F]+[lL]?"
literal|"|[-+*/<>%&^|=!]=|--|\\+\\+|<<=?|>>=?|&&|\\|\\||::|->"
argument_list|)
block|,
name|PATTERNS
argument_list|(
literal|"csharp"
argument_list|,
comment|/* Keywords */
literal|"!^[ \t]*(do|while|for|if|else|instanceof|new|return|switch|case|throw|catch|using)\n"
comment|/* Methods and constructors */
literal|"^[ \t]*(((static|public|internal|private|protected|new|virtual|sealed|override|unsafe)[ \t]+)*[][<>@.~_[:alnum:]]+[ \t]+[<>@._[:alnum:]]+[ \t]*\\(.*\\))[ \t]*$\n"
comment|/* Properties */
literal|"^[ \t]*(((static|public|internal|private|protected|new|virtual|sealed|override|unsafe)[ \t]+)*[][<>@.~_[:alnum:]]+[ \t]+[@._[:alnum:]]+)[ \t]*$\n"
comment|/* Type definitions */
literal|"^[ \t]*(((static|public|internal|private|protected|new|unsafe|sealed|abstract|partial)[ \t]+)*(class|enum|interface|struct)[ \t]+.*)$\n"
comment|/* Namespace */
literal|"^[ \t]*(namespace[ \t]+.*)$"
argument_list|,
comment|/* -- */
literal|"[a-zA-Z_][a-zA-Z0-9_]*"
literal|"|[-+0-9.e]+[fFlL]?|0[xXbB]?[0-9a-fA-F]+[lL]?"
literal|"|[-+*/<>%&^|=!]=|--|\\+\\+|<<=?|>>=?|&&|\\|\\||::|->"
argument_list|)
block|,
block|{
literal|"default"
block|,
name|NULL
block|,
operator|-
literal|1
block|,
block|{
name|NULL
block|,
literal|0
block|}
block|}
block|, }
decl_stmt|;
end_decl_stmt
begin_undef
DECL|macro|PATTERNS
undef|#
directive|undef
name|PATTERNS
end_undef
begin_undef
DECL|macro|IPATTERN
undef|#
directive|undef
name|IPATTERN
end_undef
begin_decl_stmt
DECL|variable|driver_true
specifier|static
name|struct
name|userdiff_driver
name|driver_true
init|=
block|{
literal|"diff=true"
block|,
name|NULL
block|,
literal|0
block|,
block|{
name|NULL
block|,
literal|0
block|}
block|}
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|driver_false
specifier|static
name|struct
name|userdiff_driver
name|driver_false
init|=
block|{
literal|"!diff"
block|,
name|NULL
block|,
literal|1
block|,
block|{
name|NULL
block|,
literal|0
block|}
block|}
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|userdiff_find_by_namelen
specifier|static
name|struct
name|userdiff_driver
modifier|*
name|userdiff_find_by_namelen
parameter_list|(
specifier|const
name|char
modifier|*
name|k
parameter_list|,
name|int
name|len
parameter_list|)
block|{
name|int
name|i
decl_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|ndrivers
condition|;
name|i
operator|++
control|)
block|{
name|struct
name|userdiff_driver
modifier|*
name|drv
init|=
name|drivers
operator|+
name|i
decl_stmt|;
if|if
condition|(
operator|!
name|strncmp
argument_list|(
name|drv
operator|->
name|name
argument_list|,
name|k
argument_list|,
name|len
argument_list|)
operator|&&
operator|!
name|drv
operator|->
name|name
index|[
name|len
index|]
condition|)
return|return
name|drv
return|;
block|}
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|ARRAY_SIZE
argument_list|(
name|builtin_drivers
argument_list|)
condition|;
name|i
operator|++
control|)
block|{
name|struct
name|userdiff_driver
modifier|*
name|drv
init|=
name|builtin_drivers
operator|+
name|i
decl_stmt|;
if|if
condition|(
operator|!
name|strncmp
argument_list|(
name|drv
operator|->
name|name
argument_list|,
name|k
argument_list|,
name|len
argument_list|)
operator|&&
operator|!
name|drv
operator|->
name|name
index|[
name|len
index|]
condition|)
return|return
name|drv
return|;
block|}
return|return
name|NULL
return|;
block|}
end_function
begin_function
DECL|function|parse_driver
specifier|static
name|struct
name|userdiff_driver
modifier|*
name|parse_driver
parameter_list|(
specifier|const
name|char
modifier|*
name|var
parameter_list|,
specifier|const
name|char
modifier|*
name|value
parameter_list|,
specifier|const
name|char
modifier|*
name|type
parameter_list|)
block|{
name|struct
name|userdiff_driver
modifier|*
name|drv
decl_stmt|;
specifier|const
name|char
modifier|*
name|dot
decl_stmt|;
specifier|const
name|char
modifier|*
name|name
decl_stmt|;
name|int
name|namelen
decl_stmt|;
if|if
condition|(
name|prefixcmp
argument_list|(
name|var
argument_list|,
literal|"diff."
argument_list|)
condition|)
return|return
name|NULL
return|;
name|dot
operator|=
name|strrchr
argument_list|(
name|var
argument_list|,
literal|'.'
argument_list|)
expr_stmt|;
if|if
condition|(
name|dot
operator|==
name|var
operator|+
literal|4
condition|)
return|return
name|NULL
return|;
if|if
condition|(
name|strcmp
argument_list|(
name|type
argument_list|,
name|dot
operator|+
literal|1
argument_list|)
condition|)
return|return
name|NULL
return|;
name|name
operator|=
name|var
operator|+
literal|5
expr_stmt|;
name|namelen
operator|=
name|dot
operator|-
name|name
expr_stmt|;
name|drv
operator|=
name|userdiff_find_by_namelen
argument_list|(
name|name
argument_list|,
name|namelen
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|drv
condition|)
block|{
name|ALLOC_GROW
argument_list|(
name|drivers
argument_list|,
name|ndrivers
operator|+
literal|1
argument_list|,
name|drivers_alloc
argument_list|)
expr_stmt|;
name|drv
operator|=
operator|&
name|drivers
index|[
name|ndrivers
operator|++
index|]
expr_stmt|;
name|memset
argument_list|(
name|drv
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
operator|*
name|drv
argument_list|)
argument_list|)
expr_stmt|;
name|drv
operator|->
name|name
operator|=
name|xmemdupz
argument_list|(
name|name
argument_list|,
name|namelen
argument_list|)
expr_stmt|;
name|drv
operator|->
name|binary
operator|=
operator|-
literal|1
expr_stmt|;
block|}
return|return
name|drv
return|;
block|}
end_function
begin_function
DECL|function|parse_funcname
specifier|static
name|int
name|parse_funcname
parameter_list|(
name|struct
name|userdiff_funcname
modifier|*
name|f
parameter_list|,
specifier|const
name|char
modifier|*
name|k
parameter_list|,
specifier|const
name|char
modifier|*
name|v
parameter_list|,
name|int
name|cflags
parameter_list|)
block|{
if|if
condition|(
name|git_config_string
argument_list|(
operator|&
name|f
operator|->
name|pattern
argument_list|,
name|k
argument_list|,
name|v
argument_list|)
operator|<
literal|0
condition|)
return|return
operator|-
literal|1
return|;
name|f
operator|->
name|cflags
operator|=
name|cflags
expr_stmt|;
return|return
literal|1
return|;
block|}
end_function
begin_function
DECL|function|parse_string
specifier|static
name|int
name|parse_string
parameter_list|(
specifier|const
name|char
modifier|*
modifier|*
name|d
parameter_list|,
specifier|const
name|char
modifier|*
name|k
parameter_list|,
specifier|const
name|char
modifier|*
name|v
parameter_list|)
block|{
if|if
condition|(
name|git_config_string
argument_list|(
name|d
argument_list|,
name|k
argument_list|,
name|v
argument_list|)
operator|<
literal|0
condition|)
return|return
operator|-
literal|1
return|;
return|return
literal|1
return|;
block|}
end_function
begin_function
DECL|function|parse_tristate
specifier|static
name|int
name|parse_tristate
parameter_list|(
name|int
modifier|*
name|b
parameter_list|,
specifier|const
name|char
modifier|*
name|k
parameter_list|,
specifier|const
name|char
modifier|*
name|v
parameter_list|)
block|{
if|if
condition|(
name|v
operator|&&
operator|!
name|strcasecmp
argument_list|(
name|v
argument_list|,
literal|"auto"
argument_list|)
condition|)
operator|*
name|b
operator|=
operator|-
literal|1
expr_stmt|;
else|else
operator|*
name|b
operator|=
name|git_config_bool
argument_list|(
name|k
argument_list|,
name|v
argument_list|)
expr_stmt|;
return|return
literal|1
return|;
block|}
end_function
begin_function
DECL|function|parse_bool
specifier|static
name|int
name|parse_bool
parameter_list|(
name|int
modifier|*
name|b
parameter_list|,
specifier|const
name|char
modifier|*
name|k
parameter_list|,
specifier|const
name|char
modifier|*
name|v
parameter_list|)
block|{
operator|*
name|b
operator|=
name|git_config_bool
argument_list|(
name|k
argument_list|,
name|v
argument_list|)
expr_stmt|;
return|return
literal|1
return|;
block|}
end_function
begin_function
DECL|function|userdiff_config
name|int
name|userdiff_config
parameter_list|(
specifier|const
name|char
modifier|*
name|k
parameter_list|,
specifier|const
name|char
modifier|*
name|v
parameter_list|)
block|{
name|struct
name|userdiff_driver
modifier|*
name|drv
decl_stmt|;
if|if
condition|(
operator|(
name|drv
operator|=
name|parse_driver
argument_list|(
name|k
argument_list|,
name|v
argument_list|,
literal|"funcname"
argument_list|)
operator|)
condition|)
return|return
name|parse_funcname
argument_list|(
operator|&
name|drv
operator|->
name|funcname
argument_list|,
name|k
argument_list|,
name|v
argument_list|,
literal|0
argument_list|)
return|;
if|if
condition|(
operator|(
name|drv
operator|=
name|parse_driver
argument_list|(
name|k
argument_list|,
name|v
argument_list|,
literal|"xfuncname"
argument_list|)
operator|)
condition|)
return|return
name|parse_funcname
argument_list|(
operator|&
name|drv
operator|->
name|funcname
argument_list|,
name|k
argument_list|,
name|v
argument_list|,
name|REG_EXTENDED
argument_list|)
return|;
if|if
condition|(
operator|(
name|drv
operator|=
name|parse_driver
argument_list|(
name|k
argument_list|,
name|v
argument_list|,
literal|"binary"
argument_list|)
operator|)
condition|)
return|return
name|parse_tristate
argument_list|(
operator|&
name|drv
operator|->
name|binary
argument_list|,
name|k
argument_list|,
name|v
argument_list|)
return|;
if|if
condition|(
operator|(
name|drv
operator|=
name|parse_driver
argument_list|(
name|k
argument_list|,
name|v
argument_list|,
literal|"command"
argument_list|)
operator|)
condition|)
return|return
name|parse_string
argument_list|(
operator|&
name|drv
operator|->
name|external
argument_list|,
name|k
argument_list|,
name|v
argument_list|)
return|;
if|if
condition|(
operator|(
name|drv
operator|=
name|parse_driver
argument_list|(
name|k
argument_list|,
name|v
argument_list|,
literal|"textconv"
argument_list|)
operator|)
condition|)
return|return
name|parse_string
argument_list|(
operator|&
name|drv
operator|->
name|textconv
argument_list|,
name|k
argument_list|,
name|v
argument_list|)
return|;
if|if
condition|(
operator|(
name|drv
operator|=
name|parse_driver
argument_list|(
name|k
argument_list|,
name|v
argument_list|,
literal|"cachetextconv"
argument_list|)
operator|)
condition|)
return|return
name|parse_bool
argument_list|(
operator|&
name|drv
operator|->
name|textconv_want_cache
argument_list|,
name|k
argument_list|,
name|v
argument_list|)
return|;
if|if
condition|(
operator|(
name|drv
operator|=
name|parse_driver
argument_list|(
name|k
argument_list|,
name|v
argument_list|,
literal|"wordregex"
argument_list|)
operator|)
condition|)
return|return
name|parse_string
argument_list|(
operator|&
name|drv
operator|->
name|word_regex
argument_list|,
name|k
argument_list|,
name|v
argument_list|)
return|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|userdiff_find_by_name
name|struct
name|userdiff_driver
modifier|*
name|userdiff_find_by_name
parameter_list|(
specifier|const
name|char
modifier|*
name|name
parameter_list|)
block|{
name|int
name|len
init|=
name|strlen
argument_list|(
name|name
argument_list|)
decl_stmt|;
return|return
name|userdiff_find_by_namelen
argument_list|(
name|name
argument_list|,
name|len
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|userdiff_find_by_path
name|struct
name|userdiff_driver
modifier|*
name|userdiff_find_by_path
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|)
block|{
specifier|static
name|struct
name|git_attr
modifier|*
name|attr
decl_stmt|;
name|struct
name|git_attr_check
name|check
decl_stmt|;
if|if
condition|(
operator|!
name|attr
condition|)
name|attr
operator|=
name|git_attr
argument_list|(
literal|"diff"
argument_list|)
expr_stmt|;
name|check
operator|.
name|attr
operator|=
name|attr
expr_stmt|;
if|if
condition|(
operator|!
name|path
condition|)
return|return
name|NULL
return|;
if|if
condition|(
name|git_check_attr
argument_list|(
name|path
argument_list|,
literal|1
argument_list|,
operator|&
name|check
argument_list|)
condition|)
return|return
name|NULL
return|;
if|if
condition|(
name|ATTR_TRUE
argument_list|(
name|check
operator|.
name|value
argument_list|)
condition|)
return|return
operator|&
name|driver_true
return|;
if|if
condition|(
name|ATTR_FALSE
argument_list|(
name|check
operator|.
name|value
argument_list|)
condition|)
return|return
operator|&
name|driver_false
return|;
if|if
condition|(
name|ATTR_UNSET
argument_list|(
name|check
operator|.
name|value
argument_list|)
condition|)
return|return
name|NULL
return|;
return|return
name|userdiff_find_by_name
argument_list|(
name|check
operator|.
name|value
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|userdiff_get_textconv
name|struct
name|userdiff_driver
modifier|*
name|userdiff_get_textconv
parameter_list|(
name|struct
name|userdiff_driver
modifier|*
name|driver
parameter_list|)
block|{
if|if
condition|(
operator|!
name|driver
operator|->
name|textconv
condition|)
return|return
name|NULL
return|;
if|if
condition|(
name|driver
operator|->
name|textconv_want_cache
operator|&&
operator|!
name|driver
operator|->
name|textconv_cache
condition|)
block|{
name|struct
name|notes_cache
modifier|*
name|c
init|=
name|xmalloc
argument_list|(
sizeof|sizeof
argument_list|(
operator|*
name|c
argument_list|)
argument_list|)
decl_stmt|;
name|struct
name|strbuf
name|name
init|=
name|STRBUF_INIT
decl_stmt|;
name|strbuf_addf
argument_list|(
operator|&
name|name
argument_list|,
literal|"textconv/%s"
argument_list|,
name|driver
operator|->
name|name
argument_list|)
expr_stmt|;
name|notes_cache_init
argument_list|(
name|c
argument_list|,
name|name
operator|.
name|buf
argument_list|,
name|driver
operator|->
name|textconv
argument_list|)
expr_stmt|;
name|driver
operator|->
name|textconv_cache
operator|=
name|c
expr_stmt|;
block|}
return|return
name|driver
return|;
block|}
end_function
end_unit
