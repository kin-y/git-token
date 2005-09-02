begin_unit
begin_ifndef
ifndef|#
directive|ifndef
name|CACHE_H
end_ifndef
begin_define
DECL|macro|CACHE_H
define|#
directive|define
name|CACHE_H
end_define
begin_include
include|#
directive|include
file|<unistd.h>
end_include
begin_include
include|#
directive|include
file|<stdio.h>
end_include
begin_include
include|#
directive|include
file|<sys/stat.h>
end_include
begin_include
include|#
directive|include
file|<fcntl.h>
end_include
begin_include
include|#
directive|include
file|<stddef.h>
end_include
begin_include
include|#
directive|include
file|<stdlib.h>
end_include
begin_include
include|#
directive|include
file|<stdarg.h>
end_include
begin_include
include|#
directive|include
file|<string.h>
end_include
begin_include
include|#
directive|include
file|<errno.h>
end_include
begin_include
include|#
directive|include
file|<limits.h>
end_include
begin_include
include|#
directive|include
file|<sys/mman.h>
end_include
begin_include
include|#
directive|include
file|<sys/param.h>
end_include
begin_include
include|#
directive|include
file|<netinet/in.h>
end_include
begin_include
include|#
directive|include
file|<sys/types.h>
end_include
begin_include
include|#
directive|include
file|<dirent.h>
end_include
begin_include
include|#
directive|include
include|SHA1_HEADER
end_include
begin_include
include|#
directive|include
file|<zlib.h>
end_include
begin_if
if|#
directive|if
name|ZLIB_VERNUM
operator|<
literal|0x1200
end_if
begin_define
DECL|macro|deflateBound
define|#
directive|define
name|deflateBound
parameter_list|(
name|c
parameter_list|,
name|s
parameter_list|)
value|((s) + (((s) + 7)>> 3) + (((s) + 63)>> 6) + 11)
end_define
begin_endif
endif|#
directive|endif
end_endif
begin_ifdef
ifdef|#
directive|ifdef
name|DT_UNKNOWN
end_ifdef
begin_define
DECL|macro|DTYPE
define|#
directive|define
name|DTYPE
parameter_list|(
name|de
parameter_list|)
value|((de)->d_type)
end_define
begin_else
else|#
directive|else
end_else
begin_define
DECL|macro|DT_UNKNOWN
define|#
directive|define
name|DT_UNKNOWN
value|0
end_define
begin_define
DECL|macro|DT_DIR
define|#
directive|define
name|DT_DIR
value|1
end_define
begin_define
DECL|macro|DT_REG
define|#
directive|define
name|DT_REG
value|2
end_define
begin_define
DECL|macro|DT_LNK
define|#
directive|define
name|DT_LNK
value|3
end_define
begin_define
DECL|macro|DTYPE
define|#
directive|define
name|DTYPE
parameter_list|(
name|de
parameter_list|)
value|DT_UNKNOWN
end_define
begin_endif
endif|#
directive|endif
end_endif
begin_ifdef
ifdef|#
directive|ifdef
name|__GNUC__
end_ifdef
begin_define
DECL|macro|NORETURN
define|#
directive|define
name|NORETURN
value|__attribute__((__noreturn__))
end_define
begin_else
else|#
directive|else
end_else
begin_define
DECL|macro|NORETURN
define|#
directive|define
name|NORETURN
end_define
begin_ifndef
ifndef|#
directive|ifndef
name|__attribute__
end_ifndef
begin_define
DECL|macro|__attribute__
define|#
directive|define
name|__attribute__
parameter_list|(
name|x
parameter_list|)
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
comment|/*  * Intensive research over the course of many years has shown that  * port 9418 is totally unused by anything else. Or  *  *	Your search - "port 9418" - did not match any documents.  *  * as www.google.com puts it.  */
end_comment
begin_define
DECL|macro|DEFAULT_GIT_PORT
define|#
directive|define
name|DEFAULT_GIT_PORT
value|9418
end_define
begin_comment
comment|/*  * Environment variables transition.  * We accept older names for now but warn.  */
end_comment
begin_function_decl
specifier|extern
name|char
modifier|*
name|gitenv_bc
parameter_list|(
specifier|const
name|char
modifier|*
parameter_list|)
function_decl|;
end_function_decl
begin_ifdef
ifdef|#
directive|ifdef
name|__GNUC__
end_ifdef
begin_define
DECL|macro|gitenv
define|#
directive|define
name|gitenv
parameter_list|(
name|e
parameter_list|)
value|(getenv(e) ? : gitenv_bc(e))
end_define
begin_else
else|#
directive|else
end_else
begin_define
DECL|macro|gitenv
define|#
directive|define
name|gitenv
parameter_list|(
name|e
parameter_list|)
value|(getenv(e) ? getenv(e) : gitenv_bc(e))
end_define
begin_endif
endif|#
directive|endif
end_endif
begin_comment
comment|/*  * Basic data structures for the directory cache  */
end_comment
begin_define
DECL|macro|CACHE_SIGNATURE
define|#
directive|define
name|CACHE_SIGNATURE
value|0x44495243
end_define
begin_comment
DECL|macro|CACHE_SIGNATURE
comment|/* "DIRC" */
end_comment
begin_struct
DECL|struct|cache_header
struct|struct
name|cache_header
block|{
DECL|member|hdr_signature
name|unsigned
name|int
name|hdr_signature
decl_stmt|;
DECL|member|hdr_version
name|unsigned
name|int
name|hdr_version
decl_stmt|;
DECL|member|hdr_entries
name|unsigned
name|int
name|hdr_entries
decl_stmt|;
block|}
struct|;
end_struct
begin_comment
comment|/*  * The "cache_time" is just the low 32 bits of the  * time. It doesn't matter if it overflows - we only  * check it for equality in the 32 bits we save.  */
end_comment
begin_struct
DECL|struct|cache_time
struct|struct
name|cache_time
block|{
DECL|member|sec
name|unsigned
name|int
name|sec
decl_stmt|;
DECL|member|nsec
name|unsigned
name|int
name|nsec
decl_stmt|;
block|}
struct|;
end_struct
begin_comment
comment|/*  * dev/ino/uid/gid/size are also just tracked to the low 32 bits  * Again - this is just a (very strong in practice) heuristic that  * the inode hasn't changed.  *  * We save the fields in big-endian order to allow using the  * index file over NFS transparently.  */
end_comment
begin_struct
DECL|struct|cache_entry
struct|struct
name|cache_entry
block|{
DECL|member|ce_ctime
name|struct
name|cache_time
name|ce_ctime
decl_stmt|;
DECL|member|ce_mtime
name|struct
name|cache_time
name|ce_mtime
decl_stmt|;
DECL|member|ce_dev
name|unsigned
name|int
name|ce_dev
decl_stmt|;
DECL|member|ce_ino
name|unsigned
name|int
name|ce_ino
decl_stmt|;
DECL|member|ce_mode
name|unsigned
name|int
name|ce_mode
decl_stmt|;
DECL|member|ce_uid
name|unsigned
name|int
name|ce_uid
decl_stmt|;
DECL|member|ce_gid
name|unsigned
name|int
name|ce_gid
decl_stmt|;
DECL|member|ce_size
name|unsigned
name|int
name|ce_size
decl_stmt|;
DECL|member|sha1
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
DECL|member|ce_flags
name|unsigned
name|short
name|ce_flags
decl_stmt|;
DECL|member|name
name|char
name|name
index|[
literal|0
index|]
decl_stmt|;
block|}
struct|;
end_struct
begin_define
DECL|macro|CE_NAMEMASK
define|#
directive|define
name|CE_NAMEMASK
value|(0x0fff)
end_define
begin_define
DECL|macro|CE_STAGEMASK
define|#
directive|define
name|CE_STAGEMASK
value|(0x3000)
end_define
begin_define
DECL|macro|CE_UPDATE
define|#
directive|define
name|CE_UPDATE
value|(0x4000)
end_define
begin_define
DECL|macro|CE_STAGESHIFT
define|#
directive|define
name|CE_STAGESHIFT
value|12
end_define
begin_define
DECL|macro|create_ce_flags
define|#
directive|define
name|create_ce_flags
parameter_list|(
name|len
parameter_list|,
name|stage
parameter_list|)
value|htons((len) | ((stage)<< CE_STAGESHIFT))
end_define
begin_define
DECL|macro|ce_namelen
define|#
directive|define
name|ce_namelen
parameter_list|(
name|ce
parameter_list|)
value|(CE_NAMEMASK& ntohs((ce)->ce_flags))
end_define
begin_define
DECL|macro|ce_size
define|#
directive|define
name|ce_size
parameter_list|(
name|ce
parameter_list|)
value|cache_entry_size(ce_namelen(ce))
end_define
begin_define
DECL|macro|ce_stage
define|#
directive|define
name|ce_stage
parameter_list|(
name|ce
parameter_list|)
value|((CE_STAGEMASK& ntohs((ce)->ce_flags))>> CE_STAGESHIFT)
end_define
begin_define
DECL|macro|ce_permissions
define|#
directive|define
name|ce_permissions
parameter_list|(
name|mode
parameter_list|)
value|(((mode)& 0100) ? 0755 : 0644)
end_define
begin_function
DECL|function|create_ce_mode
specifier|static
specifier|inline
name|unsigned
name|int
name|create_ce_mode
parameter_list|(
name|unsigned
name|int
name|mode
parameter_list|)
block|{
if|if
condition|(
name|S_ISLNK
argument_list|(
name|mode
argument_list|)
condition|)
return|return
name|htonl
argument_list|(
name|S_IFLNK
argument_list|)
return|;
return|return
name|htonl
argument_list|(
name|S_IFREG
operator||
name|ce_permissions
argument_list|(
name|mode
argument_list|)
argument_list|)
return|;
block|}
end_function
begin_define
DECL|macro|cache_entry_size
define|#
directive|define
name|cache_entry_size
parameter_list|(
name|len
parameter_list|)
value|((offsetof(struct cache_entry,name) + (len) + 8)& ~7)
end_define
begin_decl_stmt
specifier|extern
name|struct
name|cache_entry
modifier|*
modifier|*
name|active_cache
decl_stmt|;
end_decl_stmt
begin_decl_stmt
specifier|extern
name|unsigned
name|int
name|active_nr
decl_stmt|,
name|active_alloc
decl_stmt|,
name|active_cache_changed
decl_stmt|;
end_decl_stmt
begin_define
DECL|macro|GIT_DIR_ENVIRONMENT
define|#
directive|define
name|GIT_DIR_ENVIRONMENT
value|"GIT_DIR"
end_define
begin_define
DECL|macro|DEFAULT_GIT_DIR_ENVIRONMENT
define|#
directive|define
name|DEFAULT_GIT_DIR_ENVIRONMENT
value|".git"
end_define
begin_define
DECL|macro|DB_ENVIRONMENT
define|#
directive|define
name|DB_ENVIRONMENT
value|"GIT_OBJECT_DIRECTORY"
end_define
begin_define
DECL|macro|INDEX_ENVIRONMENT
define|#
directive|define
name|INDEX_ENVIRONMENT
value|"GIT_INDEX_FILE"
end_define
begin_define
DECL|macro|GRAFT_ENVIRONMENT
define|#
directive|define
name|GRAFT_ENVIRONMENT
value|"GIT_GRAFT_FILE"
end_define
begin_function_decl
specifier|extern
name|char
modifier|*
name|get_object_directory
parameter_list|(
name|void
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|char
modifier|*
name|get_refs_directory
parameter_list|(
name|void
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|char
modifier|*
name|get_index_file
parameter_list|(
name|void
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|char
modifier|*
name|get_graft_file
parameter_list|(
name|void
parameter_list|)
function_decl|;
end_function_decl
begin_define
DECL|macro|ALTERNATE_DB_ENVIRONMENT
define|#
directive|define
name|ALTERNATE_DB_ENVIRONMENT
value|"GIT_ALTERNATE_OBJECT_DIRECTORIES"
end_define
begin_function_decl
specifier|extern
specifier|const
name|char
modifier|*
modifier|*
name|get_pathspec
parameter_list|(
specifier|const
name|char
modifier|*
name|prefix
parameter_list|,
name|char
modifier|*
modifier|*
name|pathspec
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
specifier|const
name|char
modifier|*
name|setup_git_directory
parameter_list|(
name|void
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|char
modifier|*
name|prefix_path
parameter_list|(
specifier|const
name|char
modifier|*
name|prefix
parameter_list|,
name|int
name|len
parameter_list|,
name|char
modifier|*
name|path
parameter_list|)
function_decl|;
end_function_decl
begin_define
DECL|macro|alloc_nr
define|#
directive|define
name|alloc_nr
parameter_list|(
name|x
parameter_list|)
value|(((x)+16)*3/2)
end_define
begin_comment
comment|/* Initialize and use the cache information */
end_comment
begin_function_decl
specifier|extern
name|int
name|read_cache
parameter_list|(
name|void
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|write_cache
parameter_list|(
name|int
name|newfd
parameter_list|,
name|struct
name|cache_entry
modifier|*
modifier|*
name|cache
parameter_list|,
name|int
name|entries
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|cache_name_pos
parameter_list|(
specifier|const
name|char
modifier|*
name|name
parameter_list|,
name|int
name|namelen
parameter_list|)
function_decl|;
end_function_decl
begin_define
DECL|macro|ADD_CACHE_OK_TO_ADD
define|#
directive|define
name|ADD_CACHE_OK_TO_ADD
value|1
end_define
begin_comment
DECL|macro|ADD_CACHE_OK_TO_ADD
comment|/* Ok to add */
end_comment
begin_define
DECL|macro|ADD_CACHE_OK_TO_REPLACE
define|#
directive|define
name|ADD_CACHE_OK_TO_REPLACE
value|2
end_define
begin_comment
DECL|macro|ADD_CACHE_OK_TO_REPLACE
comment|/* Ok to replace file/directory */
end_comment
begin_define
DECL|macro|ADD_CACHE_SKIP_DFCHECK
define|#
directive|define
name|ADD_CACHE_SKIP_DFCHECK
value|4
end_define
begin_comment
DECL|macro|ADD_CACHE_SKIP_DFCHECK
comment|/* Ok to skip DF conflict checks */
end_comment
begin_function_decl
specifier|extern
name|int
name|add_cache_entry
parameter_list|(
name|struct
name|cache_entry
modifier|*
name|ce
parameter_list|,
name|int
name|option
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|remove_cache_entry_at
parameter_list|(
name|int
name|pos
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|remove_file_from_cache
parameter_list|(
name|char
modifier|*
name|path
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|ce_same_name
parameter_list|(
name|struct
name|cache_entry
modifier|*
name|a
parameter_list|,
name|struct
name|cache_entry
modifier|*
name|b
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|ce_match_stat
parameter_list|(
name|struct
name|cache_entry
modifier|*
name|ce
parameter_list|,
name|struct
name|stat
modifier|*
name|st
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|ce_path_match
parameter_list|(
specifier|const
name|struct
name|cache_entry
modifier|*
name|ce
parameter_list|,
specifier|const
name|char
modifier|*
modifier|*
name|pathspec
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|index_fd
parameter_list|(
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|,
name|int
name|fd
parameter_list|,
name|struct
name|stat
modifier|*
name|st
parameter_list|,
name|int
name|write_object
parameter_list|,
specifier|const
name|char
modifier|*
name|type
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|void
name|fill_stat_cache_info
parameter_list|(
name|struct
name|cache_entry
modifier|*
name|ce
parameter_list|,
name|struct
name|stat
modifier|*
name|st
parameter_list|)
function_decl|;
end_function_decl
begin_struct
DECL|struct|cache_file
struct|struct
name|cache_file
block|{
DECL|member|next
name|struct
name|cache_file
modifier|*
name|next
decl_stmt|;
DECL|member|lockfile
name|char
name|lockfile
index|[
name|PATH_MAX
index|]
decl_stmt|;
block|}
struct|;
end_struct
begin_function_decl
specifier|extern
name|int
name|hold_index_file_for_update
parameter_list|(
name|struct
name|cache_file
modifier|*
parameter_list|,
specifier|const
name|char
modifier|*
name|path
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|commit_index_file
parameter_list|(
name|struct
name|cache_file
modifier|*
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|void
name|rollback_index_file
parameter_list|(
name|struct
name|cache_file
modifier|*
parameter_list|)
function_decl|;
end_function_decl
begin_define
DECL|macro|MTIME_CHANGED
define|#
directive|define
name|MTIME_CHANGED
value|0x0001
end_define
begin_define
DECL|macro|CTIME_CHANGED
define|#
directive|define
name|CTIME_CHANGED
value|0x0002
end_define
begin_define
DECL|macro|OWNER_CHANGED
define|#
directive|define
name|OWNER_CHANGED
value|0x0004
end_define
begin_define
DECL|macro|MODE_CHANGED
define|#
directive|define
name|MODE_CHANGED
value|0x0008
end_define
begin_define
DECL|macro|INODE_CHANGED
define|#
directive|define
name|INODE_CHANGED
value|0x0010
end_define
begin_define
DECL|macro|DATA_CHANGED
define|#
directive|define
name|DATA_CHANGED
value|0x0020
end_define
begin_define
DECL|macro|TYPE_CHANGED
define|#
directive|define
name|TYPE_CHANGED
value|0x0040
end_define
begin_comment
comment|/* Return a statically allocated filename matching the sha1 signature */
end_comment
begin_function_decl
specifier|extern
name|char
modifier|*
name|mkpath
parameter_list|(
specifier|const
name|char
modifier|*
name|fmt
parameter_list|,
modifier|...
parameter_list|)
function_decl|__attribute__
parameter_list|(
function_decl|(format
parameter_list|(
name|printf
parameter_list|,
function_decl|1
operator|,
function_decl|2
end_function_decl
begin_empty_stmt
unit|)))
empty_stmt|;
end_empty_stmt
begin_function_decl
specifier|extern
name|char
modifier|*
name|git_path
parameter_list|(
specifier|const
name|char
modifier|*
name|fmt
parameter_list|,
modifier|...
parameter_list|)
function_decl|__attribute__
parameter_list|(
function_decl|(format
parameter_list|(
name|printf
parameter_list|,
function_decl|1
operator|,
function_decl|2
end_function_decl
begin_empty_stmt
unit|)))
empty_stmt|;
end_empty_stmt
begin_function_decl
specifier|extern
name|char
modifier|*
name|sha1_file_name
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|char
modifier|*
name|sha1_pack_name
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|char
modifier|*
name|sha1_pack_index_name
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|int
name|git_mkstemp
parameter_list|(
name|char
modifier|*
name|path
parameter_list|,
name|size_t
name|n
parameter_list|,
specifier|const
name|char
modifier|*
name|template
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|int
name|safe_create_leading_directories
parameter_list|(
name|char
modifier|*
name|path
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|char
modifier|*
name|safe_strncpy
parameter_list|(
name|char
modifier|*
parameter_list|,
specifier|const
name|char
modifier|*
parameter_list|,
name|size_t
parameter_list|)
function_decl|;
end_function_decl
begin_comment
comment|/* Read and unpack a sha1 file into memory, write memory to a sha1 file */
end_comment
begin_function_decl
specifier|extern
name|int
name|unpack_sha1_header
parameter_list|(
name|z_stream
modifier|*
name|stream
parameter_list|,
name|void
modifier|*
name|map
parameter_list|,
name|unsigned
name|long
name|mapsize
parameter_list|,
name|void
modifier|*
name|buffer
parameter_list|,
name|unsigned
name|long
name|size
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|parse_sha1_header
parameter_list|(
name|char
modifier|*
name|hdr
parameter_list|,
name|char
modifier|*
name|type
parameter_list|,
name|unsigned
name|long
modifier|*
name|sizep
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|sha1_object_info
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
parameter_list|,
name|char
modifier|*
parameter_list|,
name|unsigned
name|long
modifier|*
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|void
modifier|*
name|unpack_sha1_file
parameter_list|(
name|void
modifier|*
name|map
parameter_list|,
name|unsigned
name|long
name|mapsize
parameter_list|,
name|char
modifier|*
name|type
parameter_list|,
name|unsigned
name|long
modifier|*
name|size
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|void
modifier|*
name|read_sha1_file
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|,
name|char
modifier|*
name|type
parameter_list|,
name|unsigned
name|long
modifier|*
name|size
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|write_sha1_file
parameter_list|(
name|void
modifier|*
name|buf
parameter_list|,
name|unsigned
name|long
name|len
parameter_list|,
specifier|const
name|char
modifier|*
name|type
parameter_list|,
name|unsigned
name|char
modifier|*
name|return_sha1
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|char
modifier|*
name|write_sha1_file_prepare
parameter_list|(
name|void
modifier|*
name|buf
parameter_list|,
name|unsigned
name|long
name|len
parameter_list|,
specifier|const
name|char
modifier|*
name|type
parameter_list|,
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|,
name|unsigned
name|char
modifier|*
name|hdr
parameter_list|,
name|int
modifier|*
name|hdrlen
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|check_sha1_signature
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|,
name|void
modifier|*
name|buf
parameter_list|,
name|unsigned
name|long
name|size
parameter_list|,
specifier|const
name|char
modifier|*
name|type
parameter_list|)
function_decl|;
end_function_decl
begin_comment
comment|/* Read a tree into the cache */
end_comment
begin_function_decl
specifier|extern
name|int
name|read_tree
parameter_list|(
name|void
modifier|*
name|buffer
parameter_list|,
name|unsigned
name|long
name|size
parameter_list|,
name|int
name|stage
parameter_list|,
specifier|const
name|char
modifier|*
modifier|*
name|paths
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|write_sha1_from_fd
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|,
name|int
name|fd
parameter_list|,
name|char
modifier|*
name|buffer
parameter_list|,
name|size_t
name|bufsize
parameter_list|,
name|size_t
modifier|*
name|bufposn
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|write_sha1_to_fd
parameter_list|(
name|int
name|fd
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|has_sha1_pack
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|has_sha1_file
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|has_pack_file
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|has_pack_index
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
function_decl|;
end_function_decl
begin_comment
comment|/* Convert to/from hex/sha1 representation */
end_comment
begin_function_decl
specifier|extern
name|int
name|get_sha1
parameter_list|(
specifier|const
name|char
modifier|*
name|str
parameter_list|,
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|get_sha1_hex
parameter_list|(
specifier|const
name|char
modifier|*
name|hex
parameter_list|,
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|char
modifier|*
name|sha1_to_hex
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
function_decl|;
end_function_decl
begin_comment
comment|/* static buffer result! */
end_comment
begin_comment
comment|/* General helper functions */
end_comment
begin_decl_stmt
specifier|extern
name|void
name|usage
argument_list|(
specifier|const
name|char
operator|*
name|err
argument_list|)
name|NORETURN
decl_stmt|;
end_decl_stmt
begin_decl_stmt
specifier|extern
name|void
name|die
argument_list|(
specifier|const
name|char
operator|*
name|err
argument_list|,
operator|...
argument_list|)
name|NORETURN
name|__attribute__
argument_list|(
operator|(
name|format
argument_list|(
name|printf
argument_list|,
literal|1
argument_list|,
literal|2
argument_list|)
operator|)
argument_list|)
decl_stmt|;
end_decl_stmt
begin_function_decl
specifier|extern
name|int
name|error
parameter_list|(
specifier|const
name|char
modifier|*
name|err
parameter_list|,
modifier|...
parameter_list|)
function_decl|__attribute__
parameter_list|(
function_decl|(format
parameter_list|(
name|printf
parameter_list|,
function_decl|1
operator|,
function_decl|2
end_function_decl
begin_empty_stmt
unit|)))
empty_stmt|;
end_empty_stmt
begin_function_decl
specifier|extern
name|int
name|base_name_compare
parameter_list|(
specifier|const
name|char
modifier|*
name|name1
parameter_list|,
name|int
name|len1
parameter_list|,
name|int
name|mode1
parameter_list|,
specifier|const
name|char
modifier|*
name|name2
parameter_list|,
name|int
name|len2
parameter_list|,
name|int
name|mode2
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|cache_name_compare
parameter_list|(
specifier|const
name|char
modifier|*
name|name1
parameter_list|,
name|int
name|len1
parameter_list|,
specifier|const
name|char
modifier|*
name|name2
parameter_list|,
name|int
name|len2
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|void
modifier|*
name|read_object_with_reference
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|,
specifier|const
name|char
modifier|*
name|required_type
parameter_list|,
name|unsigned
name|long
modifier|*
name|size
parameter_list|,
name|unsigned
name|char
modifier|*
name|sha1_ret
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|const
name|char
modifier|*
name|show_date
parameter_list|(
name|unsigned
name|long
name|time
parameter_list|,
name|int
name|timezone
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|void
name|parse_date
parameter_list|(
specifier|const
name|char
modifier|*
name|date
parameter_list|,
name|char
modifier|*
name|buf
parameter_list|,
name|int
name|bufsize
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|void
name|datestamp
parameter_list|(
name|char
modifier|*
name|buf
parameter_list|,
name|int
name|bufsize
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|setup_ident
parameter_list|(
name|void
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|char
modifier|*
name|get_ident
parameter_list|(
specifier|const
name|char
modifier|*
name|name
parameter_list|,
specifier|const
name|char
modifier|*
name|email
parameter_list|,
specifier|const
name|char
modifier|*
name|date_str
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|char
modifier|*
name|git_author_info
parameter_list|(
name|void
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|char
modifier|*
name|git_committer_info
parameter_list|(
name|void
parameter_list|)
function_decl|;
end_function_decl
begin_function
DECL|function|xmalloc
specifier|static
specifier|inline
name|void
modifier|*
name|xmalloc
parameter_list|(
name|size_t
name|size
parameter_list|)
block|{
name|void
modifier|*
name|ret
init|=
name|malloc
argument_list|(
name|size
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
name|ret
condition|)
name|die
argument_list|(
literal|"Out of memory, malloc failed"
argument_list|)
expr_stmt|;
return|return
name|ret
return|;
block|}
end_function
begin_function
DECL|function|xrealloc
specifier|static
specifier|inline
name|void
modifier|*
name|xrealloc
parameter_list|(
name|void
modifier|*
name|ptr
parameter_list|,
name|size_t
name|size
parameter_list|)
block|{
name|void
modifier|*
name|ret
init|=
name|realloc
argument_list|(
name|ptr
argument_list|,
name|size
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
name|ret
condition|)
name|die
argument_list|(
literal|"Out of memory, realloc failed"
argument_list|)
expr_stmt|;
return|return
name|ret
return|;
block|}
end_function
begin_function
DECL|function|xcalloc
specifier|static
specifier|inline
name|void
modifier|*
name|xcalloc
parameter_list|(
name|size_t
name|nmemb
parameter_list|,
name|size_t
name|size
parameter_list|)
block|{
name|void
modifier|*
name|ret
init|=
name|calloc
argument_list|(
name|nmemb
argument_list|,
name|size
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
name|ret
condition|)
name|die
argument_list|(
literal|"Out of memory, calloc failed"
argument_list|)
expr_stmt|;
return|return
name|ret
return|;
block|}
end_function
begin_struct
DECL|struct|checkout
struct|struct
name|checkout
block|{
DECL|member|base_dir
specifier|const
name|char
modifier|*
name|base_dir
decl_stmt|;
DECL|member|base_dir_len
name|int
name|base_dir_len
decl_stmt|;
DECL|member|force
name|unsigned
name|force
range|:
literal|1
decl_stmt|,
DECL|member|quiet
name|quiet
range|:
literal|1
decl_stmt|,
DECL|member|not_new
name|not_new
range|:
literal|1
decl_stmt|,
DECL|member|refresh_cache
name|refresh_cache
range|:
literal|1
decl_stmt|;
block|}
struct|;
end_struct
begin_function_decl
specifier|extern
name|int
name|checkout_entry
parameter_list|(
name|struct
name|cache_entry
modifier|*
name|ce
parameter_list|,
name|struct
name|checkout
modifier|*
name|state
parameter_list|)
function_decl|;
end_function_decl
begin_struct
DECL|struct|alternate_object_database
specifier|extern
struct|struct
name|alternate_object_database
block|{
DECL|member|next
name|struct
name|alternate_object_database
modifier|*
name|next
decl_stmt|;
DECL|member|name
name|char
modifier|*
name|name
decl_stmt|;
DECL|member|base
name|char
name|base
index|[
literal|0
index|]
decl_stmt|;
comment|/* more */
block|}
modifier|*
name|alt_odb_list
struct|;
end_struct
begin_function_decl
specifier|extern
name|void
name|prepare_alt_odb
parameter_list|(
name|void
parameter_list|)
function_decl|;
end_function_decl
begin_struct
DECL|struct|packed_git
specifier|extern
struct|struct
name|packed_git
block|{
DECL|member|next
name|struct
name|packed_git
modifier|*
name|next
decl_stmt|;
DECL|member|index_size
name|unsigned
name|long
name|index_size
decl_stmt|;
DECL|member|pack_size
name|unsigned
name|long
name|pack_size
decl_stmt|;
DECL|member|index_base
name|unsigned
name|int
modifier|*
name|index_base
decl_stmt|;
DECL|member|pack_base
name|void
modifier|*
name|pack_base
decl_stmt|;
DECL|member|pack_last_used
name|unsigned
name|int
name|pack_last_used
decl_stmt|;
DECL|member|pack_use_cnt
name|unsigned
name|int
name|pack_use_cnt
decl_stmt|;
DECL|member|sha1
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
DECL|member|pack_name
name|char
name|pack_name
index|[
literal|0
index|]
decl_stmt|;
comment|/* something like ".git/objects/pack/xxxxx.pack" */
block|}
modifier|*
name|packed_git
struct|;
end_struct
begin_struct
DECL|struct|pack_entry
struct|struct
name|pack_entry
block|{
DECL|member|offset
name|unsigned
name|int
name|offset
decl_stmt|;
DECL|member|sha1
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
DECL|member|p
name|struct
name|packed_git
modifier|*
name|p
decl_stmt|;
block|}
struct|;
end_struct
begin_struct
DECL|struct|ref
struct|struct
name|ref
block|{
DECL|member|next
name|struct
name|ref
modifier|*
name|next
decl_stmt|;
DECL|member|old_sha1
name|unsigned
name|char
name|old_sha1
index|[
literal|20
index|]
decl_stmt|;
DECL|member|new_sha1
name|unsigned
name|char
name|new_sha1
index|[
literal|20
index|]
decl_stmt|;
DECL|member|force
name|unsigned
name|char
name|force
decl_stmt|;
DECL|member|peer_ref
name|struct
name|ref
modifier|*
name|peer_ref
decl_stmt|;
comment|/* when renaming */
DECL|member|name
name|char
name|name
index|[
literal|0
index|]
decl_stmt|;
block|}
struct|;
end_struct
begin_function_decl
specifier|extern
name|int
name|git_connect
parameter_list|(
name|int
name|fd
index|[
literal|2
index|]
parameter_list|,
name|char
modifier|*
name|url
parameter_list|,
specifier|const
name|char
modifier|*
name|prog
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|finish_connect
parameter_list|(
name|pid_t
name|pid
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|path_match
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|,
name|int
name|nr
parameter_list|,
name|char
modifier|*
modifier|*
name|match
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|match_refs
parameter_list|(
name|struct
name|ref
modifier|*
name|src
parameter_list|,
name|struct
name|ref
modifier|*
name|dst
parameter_list|,
name|struct
name|ref
modifier|*
modifier|*
modifier|*
name|dst_tail
parameter_list|,
name|int
name|nr_refspec
parameter_list|,
name|char
modifier|*
modifier|*
name|refspec
parameter_list|,
name|int
name|all
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|get_ack
parameter_list|(
name|int
name|fd
parameter_list|,
name|unsigned
name|char
modifier|*
name|result_sha1
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|struct
name|ref
modifier|*
modifier|*
name|get_remote_heads
parameter_list|(
name|int
name|in
parameter_list|,
name|struct
name|ref
modifier|*
modifier|*
name|list
parameter_list|,
name|int
name|nr_match
parameter_list|,
name|char
modifier|*
modifier|*
name|match
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|struct
name|packed_git
modifier|*
name|parse_pack_index
parameter_list|(
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|struct
name|packed_git
modifier|*
name|parse_pack_index_file
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|,
name|char
modifier|*
name|idx_path
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|void
name|prepare_packed_git
parameter_list|(
name|void
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|void
name|install_packed_git
parameter_list|(
name|struct
name|packed_git
modifier|*
name|pack
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|struct
name|packed_git
modifier|*
name|find_sha1_pack
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|,
name|struct
name|packed_git
modifier|*
name|packs
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|use_packed_git
parameter_list|(
name|struct
name|packed_git
modifier|*
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|void
name|unuse_packed_git
parameter_list|(
name|struct
name|packed_git
modifier|*
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|struct
name|packed_git
modifier|*
name|add_packed_git
parameter_list|(
name|char
modifier|*
parameter_list|,
name|int
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|num_packed_objects
parameter_list|(
specifier|const
name|struct
name|packed_git
modifier|*
name|p
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|nth_packed_object_sha1
parameter_list|(
specifier|const
name|struct
name|packed_git
modifier|*
parameter_list|,
name|int
parameter_list|,
name|unsigned
name|char
modifier|*
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|find_pack_entry_one
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
parameter_list|,
name|struct
name|pack_entry
modifier|*
parameter_list|,
name|struct
name|packed_git
modifier|*
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|void
modifier|*
name|unpack_entry_gently
parameter_list|(
name|struct
name|pack_entry
modifier|*
parameter_list|,
name|char
modifier|*
parameter_list|,
name|unsigned
name|long
modifier|*
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|void
name|packed_object_info_detail
parameter_list|(
name|struct
name|pack_entry
modifier|*
parameter_list|,
name|char
modifier|*
parameter_list|,
name|unsigned
name|long
modifier|*
parameter_list|,
name|unsigned
name|long
modifier|*
parameter_list|,
name|int
modifier|*
parameter_list|,
name|unsigned
name|char
modifier|*
parameter_list|)
function_decl|;
end_function_decl
begin_comment
comment|/* Dumb servers support */
end_comment
begin_function_decl
specifier|extern
name|int
name|update_server_info
parameter_list|(
name|int
parameter_list|)
function_decl|;
end_function_decl
begin_endif
endif|#
directive|endif
end_endif
begin_comment
comment|/* CACHE_H */
end_comment
end_unit
