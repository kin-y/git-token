begin_unit
comment|'#!/usr/bin/python'
nl|'\n'
comment|'#'
nl|'\n'
comment|'# p4-fast-export.py'
nl|'\n'
comment|'#'
nl|'\n'
comment|'# Author: Simon Hausmann <hausmann@kde.org>'
nl|'\n'
comment|'# License: MIT <http://www.opensource.org/licenses/mit-license.php>'
nl|'\n'
comment|'#'
nl|'\n'
comment|'# TODO:'
nl|'\n'
comment|'#       - support integrations (at least p4i)'
nl|'\n'
comment|'#       - support p4 submit (hah!)'
nl|'\n'
comment|"#       - emulate p4's delete behavior: if a directory becomes empty delete it. continue"
nl|'\n'
comment|'#         with parent dir until non-empty dir is found.'
nl|'\n'
comment|'#'
nl|'\n'
name|'import'
name|'os'
op|','
name|'string'
op|','
name|'sys'
op|','
name|'time'
newline|'\n'
name|'import'
name|'marshal'
op|','
name|'popen2'
op|','
name|'getopt'
newline|'\n'
name|'from'
name|'sets'
name|'import'
name|'Set'
op|';'
newline|'\n'
nl|'\n'
DECL|variable|silent
name|'silent'
op|'='
name|'False'
newline|'\n'
DECL|variable|knownBranches
name|'knownBranches'
op|'='
name|'Set'
op|'('
op|')'
newline|'\n'
DECL|variable|committedChanges
name|'committedChanges'
op|'='
name|'Set'
op|'('
op|')'
newline|'\n'
DECL|variable|branch
name|'branch'
op|'='
string|'"refs/heads/master"'
newline|'\n'
name|'globalPrefix'
op|'='
name|'previousDepotPath'
op|'='
name|'os'
op|'.'
name|'popen'
op|'('
string|'"git-repo-config --get p4.depotpath"'
op|')'
op|'.'
name|'read'
op|'('
op|')'
newline|'\n'
DECL|variable|detectBranches
name|'detectBranches'
op|'='
name|'False'
newline|'\n'
DECL|variable|changesFile
name|'changesFile'
op|'='
string|'""'
newline|'\n'
name|'if'
name|'len'
op|'('
name|'globalPrefix'
op|')'
op|'!='
number|'0'
op|':'
newline|'\n'
DECL|variable|globalPrefix
indent|'    '
name|'globalPrefix'
op|'='
name|'globalPrefix'
op|'['
op|':'
op|'-'
number|'1'
op|']'
newline|'\n'
nl|'\n'
dedent|''
name|'try'
op|':'
newline|'\n'
indent|'    '
name|'opts'
op|','
name|'args'
op|'='
name|'getopt'
op|'.'
name|'getopt'
op|'('
name|'sys'
op|'.'
name|'argv'
op|'['
number|'1'
op|':'
op|']'
op|','
string|'""'
op|','
op|'['
string|'"branch="'
op|','
string|'"detect-branches"'
op|','
string|'"changesfile="'
op|','
string|'"silent"'
op|']'
op|')'
newline|'\n'
dedent|''
name|'except'
name|'getopt'
op|'.'
name|'GetoptError'
op|':'
newline|'\n'
indent|'    '
name|'print'
string|'"fixme, syntax error"'
newline|'\n'
name|'sys'
op|'.'
name|'exit'
op|'('
number|'1'
op|')'
newline|'\n'
nl|'\n'
dedent|''
name|'for'
name|'o'
op|','
name|'a'
name|'in'
name|'opts'
op|':'
newline|'\n'
indent|'    '
name|'if'
name|'o'
op|'=='
string|'"--branch"'
op|':'
newline|'\n'
DECL|variable|branch
indent|'        '
name|'branch'
op|'='
string|'"refs/heads/"'
op|'+'
name|'a'
newline|'\n'
dedent|''
name|'elif'
name|'o'
op|'=='
string|'"--detect-branches"'
op|':'
newline|'\n'
DECL|variable|detectBranches
indent|'        '
name|'detectBranches'
op|'='
name|'True'
newline|'\n'
dedent|''
name|'elif'
name|'o'
op|'=='
string|'"--changesfile"'
op|':'
newline|'\n'
DECL|variable|changesFile
indent|'        '
name|'changesFile'
op|'='
name|'a'
newline|'\n'
dedent|''
name|'elif'
name|'o'
op|'=='
string|'"--silent"'
op|':'
newline|'\n'
DECL|variable|silent
indent|'        '
name|'silent'
op|'='
name|'True'
newline|'\n'
nl|'\n'
dedent|''
dedent|''
name|'if'
name|'len'
op|'('
name|'args'
op|')'
op|'=='
number|'0'
name|'and'
name|'len'
op|'('
name|'globalPrefix'
op|')'
op|'!='
number|'0'
op|':'
newline|'\n'
indent|'    '
name|'print'
string|'"[using previously specified depot path %s]"'
op|'%'
name|'globalPrefix'
newline|'\n'
dedent|''
name|'elif'
name|'len'
op|'('
name|'args'
op|')'
op|'!='
number|'1'
op|':'
newline|'\n'
indent|'    '
name|'print'
string|'"usage: %s //depot/path[@revRange]"'
op|'%'
name|'sys'
op|'.'
name|'argv'
op|'['
number|'0'
op|']'
newline|'\n'
name|'print'
string|'"\\n    example:"'
newline|'\n'
name|'print'
string|'"    %s //depot/my/project/ -- to import the current head"'
newline|'\n'
name|'print'
string|'"    %s //depot/my/project/@all -- to import everything"'
newline|'\n'
name|'print'
string|'"    %s //depot/my/project/@1,6 -- to import only from revision 1 to 6"'
newline|'\n'
name|'print'
string|'""'
newline|'\n'
name|'print'
string|'"    (a ... is not needed in the path p4 specification, it\'s added implicitly)"'
newline|'\n'
name|'print'
string|'""'
newline|'\n'
name|'sys'
op|'.'
name|'exit'
op|'('
number|'1'
op|')'
newline|'\n'
dedent|''
name|'else'
op|':'
newline|'\n'
indent|'    '
name|'if'
name|'len'
op|'('
name|'globalPrefix'
op|')'
op|'!='
number|'0'
name|'and'
name|'globalPrefix'
op|'!='
name|'args'
op|'['
number|'0'
op|']'
op|':'
newline|'\n'
indent|'        '
name|'print'
string|'"previous import used depot path %s and now %s was specified. this doesn\'t work!"'
op|'%'
op|'('
name|'globalPrefix'
op|','
name|'args'
op|'['
number|'0'
op|']'
op|')'
newline|'\n'
name|'sys'
op|'.'
name|'exit'
op|'('
number|'1'
op|')'
newline|'\n'
DECL|variable|globalPrefix
dedent|''
name|'globalPrefix'
op|'='
name|'args'
op|'['
number|'0'
op|']'
newline|'\n'
nl|'\n'
DECL|variable|changeRange
dedent|''
name|'changeRange'
op|'='
string|'""'
newline|'\n'
DECL|variable|revision
name|'revision'
op|'='
string|'""'
newline|'\n'
DECL|variable|users
name|'users'
op|'='
op|'{'
op|'}'
newline|'\n'
DECL|variable|initialParent
name|'initialParent'
op|'='
string|'""'
newline|'\n'
DECL|variable|lastChange
name|'lastChange'
op|'='
number|'0'
newline|'\n'
DECL|variable|initialTag
name|'initialTag'
op|'='
string|'""'
newline|'\n'
nl|'\n'
name|'if'
name|'globalPrefix'
op|'.'
name|'find'
op|'('
string|'"@"'
op|')'
op|'!='
op|'-'
number|'1'
op|':'
newline|'\n'
DECL|variable|atIdx
indent|'    '
name|'atIdx'
op|'='
name|'globalPrefix'
op|'.'
name|'index'
op|'('
string|'"@"'
op|')'
newline|'\n'
DECL|variable|changeRange
name|'changeRange'
op|'='
name|'globalPrefix'
op|'['
name|'atIdx'
op|':'
op|']'
newline|'\n'
name|'if'
name|'changeRange'
op|'=='
string|'"@all"'
op|':'
newline|'\n'
DECL|variable|changeRange
indent|'        '
name|'changeRange'
op|'='
string|'""'
newline|'\n'
dedent|''
name|'elif'
name|'changeRange'
op|'.'
name|'find'
op|'('
string|'","'
op|')'
op|'=='
op|'-'
number|'1'
op|':'
newline|'\n'
DECL|variable|revision
indent|'        '
name|'revision'
op|'='
name|'changeRange'
newline|'\n'
DECL|variable|changeRange
name|'changeRange'
op|'='
string|'""'
newline|'\n'
DECL|variable|globalPrefix
dedent|''
name|'globalPrefix'
op|'='
name|'globalPrefix'
op|'['
number|'0'
op|':'
name|'atIdx'
op|']'
newline|'\n'
dedent|''
name|'elif'
name|'globalPrefix'
op|'.'
name|'find'
op|'('
string|'"#"'
op|')'
op|'!='
op|'-'
number|'1'
op|':'
newline|'\n'
DECL|variable|hashIdx
indent|'    '
name|'hashIdx'
op|'='
name|'globalPrefix'
op|'.'
name|'index'
op|'('
string|'"#"'
op|')'
newline|'\n'
DECL|variable|revision
name|'revision'
op|'='
name|'globalPrefix'
op|'['
name|'hashIdx'
op|':'
op|']'
newline|'\n'
DECL|variable|globalPrefix
name|'globalPrefix'
op|'='
name|'globalPrefix'
op|'['
number|'0'
op|':'
name|'hashIdx'
op|']'
newline|'\n'
dedent|''
name|'elif'
name|'len'
op|'('
name|'previousDepotPath'
op|')'
op|'=='
number|'0'
op|':'
newline|'\n'
DECL|variable|revision
indent|'    '
name|'revision'
op|'='
string|'"#head"'
newline|'\n'
nl|'\n'
dedent|''
name|'if'
name|'globalPrefix'
op|'.'
name|'endswith'
op|'('
string|'"..."'
op|')'
op|':'
newline|'\n'
DECL|variable|globalPrefix
indent|'    '
name|'globalPrefix'
op|'='
name|'globalPrefix'
op|'['
op|':'
op|'-'
number|'3'
op|']'
newline|'\n'
nl|'\n'
dedent|''
name|'if'
name|'not'
name|'globalPrefix'
op|'.'
name|'endswith'
op|'('
string|'"/"'
op|')'
op|':'
newline|'\n'
indent|'    '
name|'globalPrefix'
op|'+='
string|'"/"'
newline|'\n'
nl|'\n'
DECL|function|p4CmdList
dedent|''
name|'def'
name|'p4CmdList'
op|'('
name|'cmd'
op|')'
op|':'
newline|'\n'
indent|'    '
name|'pipe'
op|'='
name|'os'
op|'.'
name|'popen'
op|'('
string|'"p4 -G %s"'
op|'%'
name|'cmd'
op|','
string|'"rb"'
op|')'
newline|'\n'
name|'result'
op|'='
op|'['
op|']'
newline|'\n'
name|'try'
op|':'
newline|'\n'
indent|'        '
name|'while'
name|'True'
op|':'
newline|'\n'
indent|'            '
name|'entry'
op|'='
name|'marshal'
op|'.'
name|'load'
op|'('
name|'pipe'
op|')'
newline|'\n'
name|'result'
op|'.'
name|'append'
op|'('
name|'entry'
op|')'
newline|'\n'
dedent|''
dedent|''
name|'except'
name|'EOFError'
op|':'
newline|'\n'
indent|'        '
name|'pass'
newline|'\n'
dedent|''
name|'pipe'
op|'.'
name|'close'
op|'('
op|')'
newline|'\n'
name|'return'
name|'result'
newline|'\n'
nl|'\n'
DECL|function|p4Cmd
dedent|''
name|'def'
name|'p4Cmd'
op|'('
name|'cmd'
op|')'
op|':'
newline|'\n'
indent|'    '
name|'list'
op|'='
name|'p4CmdList'
op|'('
name|'cmd'
op|')'
newline|'\n'
name|'result'
op|'='
op|'{'
op|'}'
newline|'\n'
name|'for'
name|'entry'
name|'in'
name|'list'
op|':'
newline|'\n'
indent|'        '
name|'result'
op|'.'
name|'update'
op|'('
name|'entry'
op|')'
newline|'\n'
dedent|''
name|'return'
name|'result'
op|';'
newline|'\n'
nl|'\n'
DECL|function|extractFilesFromCommit
dedent|''
name|'def'
name|'extractFilesFromCommit'
op|'('
name|'commit'
op|')'
op|':'
newline|'\n'
indent|'    '
name|'files'
op|'='
op|'['
op|']'
newline|'\n'
name|'fnum'
op|'='
number|'0'
newline|'\n'
name|'while'
name|'commit'
op|'.'
name|'has_key'
op|'('
string|'"depotFile%s"'
op|'%'
name|'fnum'
op|')'
op|':'
newline|'\n'
indent|'        '
name|'path'
op|'='
name|'commit'
op|'['
string|'"depotFile%s"'
op|'%'
name|'fnum'
op|']'
newline|'\n'
name|'if'
name|'not'
name|'path'
op|'.'
name|'startswith'
op|'('
name|'globalPrefix'
op|')'
op|':'
newline|'\n'
indent|'            '
name|'if'
name|'not'
name|'silent'
op|':'
newline|'\n'
indent|'                '
name|'print'
string|'"\\nchanged files: ignoring path %s outside of %s in change %s"'
op|'%'
op|'('
name|'path'
op|','
name|'globalPrefix'
op|','
name|'change'
op|')'
newline|'\n'
dedent|''
name|'fnum'
op|'='
name|'fnum'
op|'+'
number|'1'
newline|'\n'
name|'continue'
newline|'\n'
nl|'\n'
dedent|''
name|'file'
op|'='
op|'{'
op|'}'
newline|'\n'
name|'file'
op|'['
string|'"path"'
op|']'
op|'='
name|'path'
newline|'\n'
name|'file'
op|'['
string|'"rev"'
op|']'
op|'='
name|'commit'
op|'['
string|'"rev%s"'
op|'%'
name|'fnum'
op|']'
newline|'\n'
name|'file'
op|'['
string|'"action"'
op|']'
op|'='
name|'commit'
op|'['
string|'"action%s"'
op|'%'
name|'fnum'
op|']'
newline|'\n'
name|'file'
op|'['
string|'"type"'
op|']'
op|'='
name|'commit'
op|'['
string|'"type%s"'
op|'%'
name|'fnum'
op|']'
newline|'\n'
name|'files'
op|'.'
name|'append'
op|'('
name|'file'
op|')'
newline|'\n'
name|'fnum'
op|'='
name|'fnum'
op|'+'
number|'1'
newline|'\n'
dedent|''
name|'return'
name|'files'
newline|'\n'
nl|'\n'
DECL|function|isSubPathOf
dedent|''
name|'def'
name|'isSubPathOf'
op|'('
name|'first'
op|','
name|'second'
op|')'
op|':'
newline|'\n'
indent|'    '
name|'if'
name|'not'
name|'first'
op|'.'
name|'startswith'
op|'('
name|'second'
op|')'
op|':'
newline|'\n'
indent|'        '
name|'return'
name|'False'
newline|'\n'
dedent|''
name|'if'
name|'first'
op|'=='
name|'second'
op|':'
newline|'\n'
indent|'        '
name|'return'
name|'True'
newline|'\n'
dedent|''
name|'return'
name|'first'
op|'['
name|'len'
op|'('
name|'second'
op|')'
op|']'
op|'=='
string|'"/"'
newline|'\n'
nl|'\n'
DECL|function|branchesForCommit
dedent|''
name|'def'
name|'branchesForCommit'
op|'('
name|'files'
op|')'
op|':'
newline|'\n'
indent|'    '
name|'global'
name|'knownBranches'
newline|'\n'
name|'branches'
op|'='
name|'Set'
op|'('
op|')'
newline|'\n'
nl|'\n'
name|'for'
name|'file'
name|'in'
name|'files'
op|':'
newline|'\n'
indent|'        '
name|'relativePath'
op|'='
name|'file'
op|'['
string|'"path"'
op|']'
op|'['
name|'len'
op|'('
name|'globalPrefix'
op|')'
op|':'
op|']'
newline|'\n'
comment|'# strip off the filename'
nl|'\n'
name|'relativePath'
op|'='
name|'relativePath'
op|'['
number|'0'
op|':'
name|'relativePath'
op|'.'
name|'rfind'
op|'('
string|'"/"'
op|')'
op|']'
newline|'\n'
nl|'\n'
comment|'#        if len(branches) == 0:'
nl|'\n'
comment|'#            branches.add(relativePath)'
nl|'\n'
comment|'#            knownBranches.add(relativePath)'
nl|'\n'
comment|'#            continue'
nl|'\n'
nl|'\n'
comment|'###### this needs more testing :)'
nl|'\n'
name|'knownBranch'
op|'='
name|'False'
newline|'\n'
name|'for'
name|'branch'
name|'in'
name|'branches'
op|':'
newline|'\n'
indent|'            '
name|'if'
name|'relativePath'
op|'=='
name|'branch'
op|':'
newline|'\n'
indent|'                '
name|'knownBranch'
op|'='
name|'True'
newline|'\n'
name|'break'
newline|'\n'
comment|'#            if relativePath.startswith(branch):'
nl|'\n'
dedent|''
name|'if'
name|'isSubPathOf'
op|'('
name|'relativePath'
op|','
name|'branch'
op|')'
op|':'
newline|'\n'
indent|'                '
name|'knownBranch'
op|'='
name|'True'
newline|'\n'
name|'break'
newline|'\n'
comment|'#            if branch.startswith(relativePath):'
nl|'\n'
dedent|''
name|'if'
name|'isSubPathOf'
op|'('
name|'branch'
op|','
name|'relativePath'
op|')'
op|':'
newline|'\n'
indent|'                '
name|'branches'
op|'.'
name|'remove'
op|'('
name|'branch'
op|')'
newline|'\n'
name|'break'
newline|'\n'
nl|'\n'
dedent|''
dedent|''
name|'if'
name|'knownBranch'
op|':'
newline|'\n'
indent|'            '
name|'continue'
newline|'\n'
nl|'\n'
dedent|''
name|'for'
name|'branch'
name|'in'
name|'knownBranches'
op|':'
newline|'\n'
comment|'#if relativePath.startswith(branch):'
nl|'\n'
indent|'            '
name|'if'
name|'isSubPathOf'
op|'('
name|'relativePath'
op|','
name|'branch'
op|')'
op|':'
newline|'\n'
indent|'                '
name|'if'
name|'len'
op|'('
name|'branches'
op|')'
op|'=='
number|'0'
op|':'
newline|'\n'
indent|'                    '
name|'relativePath'
op|'='
name|'branch'
newline|'\n'
dedent|''
name|'else'
op|':'
newline|'\n'
indent|'                    '
name|'knownBranch'
op|'='
name|'True'
newline|'\n'
dedent|''
name|'break'
newline|'\n'
nl|'\n'
dedent|''
dedent|''
name|'if'
name|'knownBranch'
op|':'
newline|'\n'
indent|'            '
name|'continue'
newline|'\n'
nl|'\n'
dedent|''
name|'branches'
op|'.'
name|'add'
op|'('
name|'relativePath'
op|')'
newline|'\n'
name|'knownBranches'
op|'.'
name|'add'
op|'('
name|'relativePath'
op|')'
newline|'\n'
nl|'\n'
dedent|''
name|'return'
name|'branches'
newline|'\n'
nl|'\n'
DECL|function|commit
dedent|''
name|'def'
name|'commit'
op|'('
name|'details'
op|','
name|'files'
op|','
name|'branch'
op|','
name|'branchPrefix'
op|')'
op|':'
newline|'\n'
indent|'    '
name|'global'
name|'initialParent'
newline|'\n'
name|'global'
name|'users'
newline|'\n'
name|'global'
name|'lastChange'
newline|'\n'
name|'global'
name|'committedChanges'
newline|'\n'
nl|'\n'
name|'epoch'
op|'='
name|'details'
op|'['
string|'"time"'
op|']'
newline|'\n'
name|'author'
op|'='
name|'details'
op|'['
string|'"user"'
op|']'
newline|'\n'
nl|'\n'
name|'gitStream'
op|'.'
name|'write'
op|'('
string|'"commit %s\\n"'
op|'%'
name|'branch'
op|')'
newline|'\n'
name|'gitStream'
op|'.'
name|'write'
op|'('
string|'"mark :%s\\n"'
op|'%'
name|'details'
op|'['
string|'"change"'
op|']'
op|')'
newline|'\n'
name|'committedChanges'
op|'.'
name|'add'
op|'('
name|'int'
op|'('
name|'details'
op|'['
string|'"change"'
op|']'
op|')'
op|')'
newline|'\n'
name|'committer'
op|'='
string|'""'
newline|'\n'
name|'if'
name|'author'
name|'in'
name|'users'
op|':'
newline|'\n'
indent|'        '
name|'committer'
op|'='
string|'"%s %s %s"'
op|'%'
op|'('
name|'users'
op|'['
name|'author'
op|']'
op|','
name|'epoch'
op|','
name|'tz'
op|')'
newline|'\n'
dedent|''
name|'else'
op|':'
newline|'\n'
indent|'        '
name|'committer'
op|'='
string|'"%s <a@b> %s %s"'
op|'%'
op|'('
name|'author'
op|','
name|'epoch'
op|','
name|'tz'
op|')'
newline|'\n'
nl|'\n'
dedent|''
name|'gitStream'
op|'.'
name|'write'
op|'('
string|'"committer %s\\n"'
op|'%'
name|'committer'
op|')'
newline|'\n'
nl|'\n'
name|'gitStream'
op|'.'
name|'write'
op|'('
string|'"data <<EOT\\n"'
op|')'
newline|'\n'
name|'gitStream'
op|'.'
name|'write'
op|'('
name|'details'
op|'['
string|'"desc"'
op|']'
op|')'
newline|'\n'
name|'gitStream'
op|'.'
name|'write'
op|'('
string|'"\\n[ imported from %s; change %s ]\\n"'
op|'%'
op|'('
name|'branchPrefix'
op|','
name|'details'
op|'['
string|'"change"'
op|']'
op|')'
op|')'
newline|'\n'
name|'gitStream'
op|'.'
name|'write'
op|'('
string|'"EOT\\n\\n"'
op|')'
newline|'\n'
nl|'\n'
name|'if'
name|'len'
op|'('
name|'initialParent'
op|')'
op|'>'
number|'0'
op|':'
newline|'\n'
indent|'        '
name|'gitStream'
op|'.'
name|'write'
op|'('
string|'"from %s\\n"'
op|'%'
name|'initialParent'
op|')'
newline|'\n'
name|'initialParent'
op|'='
string|'""'
newline|'\n'
nl|'\n'
comment|'#mergedBranches = Set()'
nl|'\n'
dedent|''
name|'merges'
op|'='
name|'Set'
op|'('
op|')'
newline|'\n'
nl|'\n'
name|'for'
name|'file'
name|'in'
name|'files'
op|':'
newline|'\n'
indent|'        '
name|'if'
name|'lastChange'
op|'=='
number|'0'
name|'or'
name|'not'
name|'detectBranches'
op|':'
newline|'\n'
indent|'            '
name|'continue'
newline|'\n'
dedent|''
name|'path'
op|'='
name|'file'
op|'['
string|'"path"'
op|']'
newline|'\n'
name|'if'
name|'not'
name|'path'
op|'.'
name|'startswith'
op|'('
name|'branchPrefix'
op|')'
op|':'
newline|'\n'
indent|'            '
name|'continue'
newline|'\n'
dedent|''
name|'action'
op|'='
name|'file'
op|'['
string|'"action"'
op|']'
newline|'\n'
name|'if'
name|'action'
op|'!='
string|'"integrate"'
name|'and'
name|'action'
op|'!='
string|'"branch"'
op|':'
newline|'\n'
indent|'            '
name|'continue'
newline|'\n'
dedent|''
name|'rev'
op|'='
name|'file'
op|'['
string|'"rev"'
op|']'
newline|'\n'
name|'depotPath'
op|'='
name|'path'
op|'+'
string|'"#"'
op|'+'
name|'rev'
newline|'\n'
nl|'\n'
name|'log'
op|'='
name|'p4CmdList'
op|'('
string|'"filelog \\"%s\\""'
op|'%'
name|'depotPath'
op|')'
newline|'\n'
name|'if'
name|'len'
op|'('
name|'log'
op|')'
op|'!='
number|'1'
op|':'
newline|'\n'
indent|'            '
name|'print'
string|'"eek! I got confused by the filelog of %s"'
op|'%'
name|'depotPath'
newline|'\n'
name|'sys'
op|'.'
name|'exit'
op|'('
number|'1'
op|')'
op|';'
newline|'\n'
nl|'\n'
dedent|''
name|'log'
op|'='
name|'log'
op|'['
number|'0'
op|']'
newline|'\n'
name|'if'
name|'log'
op|'['
string|'"action0"'
op|']'
op|'!='
name|'action'
op|':'
newline|'\n'
indent|'            '
name|'print'
string|'"eek! wrong action in filelog for %s : found %s, expected %s"'
op|'%'
op|'('
name|'depotPath'
op|','
name|'log'
op|'['
string|'"action0"'
op|']'
op|','
name|'action'
op|')'
newline|'\n'
name|'sys'
op|'.'
name|'exit'
op|'('
number|'1'
op|')'
op|';'
newline|'\n'
nl|'\n'
dedent|''
name|'branchAction'
op|'='
name|'log'
op|'['
string|'"how0,0"'
op|']'
newline|'\n'
comment|'#        if branchAction == "branch into" or branchAction == "ignored":'
nl|'\n'
comment|'#            continue # ignore for branching'
nl|'\n'
nl|'\n'
name|'if'
name|'not'
name|'branchAction'
op|'.'
name|'endswith'
op|'('
string|'" from"'
op|')'
op|':'
newline|'\n'
indent|'            '
name|'continue'
comment|'# ignore for branching'
newline|'\n'
comment|'#            print "eek! file %s was not branched from but instead: %s" % (depotPath, branchAction)'
nl|'\n'
comment|'#            sys.exit(1);'
nl|'\n'
nl|'\n'
dedent|''
name|'source'
op|'='
name|'log'
op|'['
string|'"file0,0"'
op|']'
newline|'\n'
name|'if'
name|'source'
op|'.'
name|'startswith'
op|'('
name|'branchPrefix'
op|')'
op|':'
newline|'\n'
indent|'            '
name|'continue'
newline|'\n'
nl|'\n'
dedent|''
name|'lastSourceRev'
op|'='
name|'log'
op|'['
string|'"erev0,0"'
op|']'
newline|'\n'
nl|'\n'
name|'sourceLog'
op|'='
name|'p4CmdList'
op|'('
string|'"filelog -m 1 \\"%s%s\\""'
op|'%'
op|'('
name|'source'
op|','
name|'lastSourceRev'
op|')'
op|')'
newline|'\n'
name|'if'
name|'len'
op|'('
name|'sourceLog'
op|')'
op|'!='
number|'1'
op|':'
newline|'\n'
indent|'            '
name|'print'
string|'"eek! I got confused by the source filelog of %s%s"'
op|'%'
op|'('
name|'source'
op|','
name|'lastSourceRev'
op|')'
newline|'\n'
name|'sys'
op|'.'
name|'exit'
op|'('
number|'1'
op|')'
op|';'
newline|'\n'
dedent|''
name|'sourceLog'
op|'='
name|'sourceLog'
op|'['
number|'0'
op|']'
newline|'\n'
nl|'\n'
name|'change'
op|'='
name|'int'
op|'('
name|'sourceLog'
op|'['
string|'"change0"'
op|']'
op|')'
newline|'\n'
name|'merges'
op|'.'
name|'add'
op|'('
name|'change'
op|')'
newline|'\n'
nl|'\n'
comment|'#        relPath = source[len(globalPrefix):]'
nl|'\n'
comment|'#'
nl|'\n'
comment|'#        for branch in knownBranches:'
nl|'\n'
comment|'#            if relPath.startswith(branch) and branch not in mergedBranches:'
nl|'\n'
comment|'#                gitStream.write("merge refs/heads/%s\\n" % branch)'
nl|'\n'
comment|'#                mergedBranches.add(branch)'
nl|'\n'
comment|'#                break'
nl|'\n'
nl|'\n'
dedent|''
name|'for'
name|'merge'
name|'in'
name|'merges'
op|':'
newline|'\n'
indent|'        '
name|'if'
name|'merge'
name|'in'
name|'committedChanges'
op|':'
newline|'\n'
indent|'            '
name|'gitStream'
op|'.'
name|'write'
op|'('
string|'"merge :%s\\n"'
op|'%'
name|'merge'
op|')'
newline|'\n'
nl|'\n'
dedent|''
dedent|''
name|'for'
name|'file'
name|'in'
name|'files'
op|':'
newline|'\n'
indent|'        '
name|'path'
op|'='
name|'file'
op|'['
string|'"path"'
op|']'
newline|'\n'
name|'if'
name|'not'
name|'path'
op|'.'
name|'startswith'
op|'('
name|'branchPrefix'
op|')'
op|':'
newline|'\n'
indent|'            '
name|'if'
name|'not'
name|'silent'
op|':'
newline|'\n'
indent|'                '
name|'print'
string|'"\\nchanged files: ignoring path %s outside of branch prefix %s in change %s"'
op|'%'
op|'('
name|'path'
op|','
name|'branchPrefix'
op|','
name|'details'
op|'['
string|'"change"'
op|']'
op|')'
newline|'\n'
dedent|''
name|'continue'
newline|'\n'
dedent|''
name|'rev'
op|'='
name|'file'
op|'['
string|'"rev"'
op|']'
newline|'\n'
name|'depotPath'
op|'='
name|'path'
op|'+'
string|'"#"'
op|'+'
name|'rev'
newline|'\n'
name|'relPath'
op|'='
name|'path'
op|'['
name|'len'
op|'('
name|'branchPrefix'
op|')'
op|':'
op|']'
newline|'\n'
name|'action'
op|'='
name|'file'
op|'['
string|'"action"'
op|']'
newline|'\n'
nl|'\n'
name|'if'
name|'action'
op|'=='
string|'"delete"'
op|':'
newline|'\n'
indent|'            '
name|'gitStream'
op|'.'
name|'write'
op|'('
string|'"D %s\\n"'
op|'%'
name|'relPath'
op|')'
newline|'\n'
dedent|''
name|'else'
op|':'
newline|'\n'
indent|'            '
name|'mode'
op|'='
number|'644'
newline|'\n'
name|'if'
name|'file'
op|'['
string|'"type"'
op|']'
op|'.'
name|'startswith'
op|'('
string|'"x"'
op|')'
op|':'
newline|'\n'
indent|'                '
name|'mode'
op|'='
number|'755'
newline|'\n'
nl|'\n'
dedent|''
name|'data'
op|'='
name|'os'
op|'.'
name|'popen'
op|'('
string|'"p4 print -q \\"%s\\""'
op|'%'
name|'depotPath'
op|','
string|'"rb"'
op|')'
op|'.'
name|'read'
op|'('
op|')'
newline|'\n'
nl|'\n'
name|'gitStream'
op|'.'
name|'write'
op|'('
string|'"M %s inline %s\\n"'
op|'%'
op|'('
name|'mode'
op|','
name|'relPath'
op|')'
op|')'
newline|'\n'
name|'gitStream'
op|'.'
name|'write'
op|'('
string|'"data %s\\n"'
op|'%'
name|'len'
op|'('
name|'data'
op|')'
op|')'
newline|'\n'
name|'gitStream'
op|'.'
name|'write'
op|'('
name|'data'
op|')'
newline|'\n'
name|'gitStream'
op|'.'
name|'write'
op|'('
string|'"\\n"'
op|')'
newline|'\n'
nl|'\n'
dedent|''
dedent|''
name|'gitStream'
op|'.'
name|'write'
op|'('
string|'"\\n"'
op|')'
newline|'\n'
nl|'\n'
name|'lastChange'
op|'='
name|'int'
op|'('
name|'details'
op|'['
string|'"change"'
op|']'
op|')'
newline|'\n'
nl|'\n'
DECL|function|getUserMap
dedent|''
name|'def'
name|'getUserMap'
op|'('
op|')'
op|':'
newline|'\n'
indent|'    '
name|'users'
op|'='
op|'{'
op|'}'
newline|'\n'
nl|'\n'
name|'for'
name|'output'
name|'in'
name|'p4CmdList'
op|'('
string|'"users"'
op|')'
op|':'
newline|'\n'
indent|'        '
name|'if'
name|'not'
name|'output'
op|'.'
name|'has_key'
op|'('
string|'"User"'
op|')'
op|':'
newline|'\n'
indent|'            '
name|'continue'
newline|'\n'
dedent|''
name|'users'
op|'['
name|'output'
op|'['
string|'"User"'
op|']'
op|']'
op|'='
name|'output'
op|'['
string|'"FullName"'
op|']'
op|'+'
string|'" <"'
op|'+'
name|'output'
op|'['
string|'"Email"'
op|']'
op|'+'
string|'">"'
newline|'\n'
dedent|''
name|'return'
name|'users'
newline|'\n'
nl|'\n'
DECL|variable|users
dedent|''
name|'users'
op|'='
name|'getUserMap'
op|'('
op|')'
newline|'\n'
nl|'\n'
name|'if'
name|'len'
op|'('
name|'changeRange'
op|')'
op|'=='
number|'0'
op|':'
newline|'\n'
indent|'    '
name|'try'
op|':'
newline|'\n'
indent|'        '
name|'sout'
op|','
name|'sin'
op|','
name|'serr'
op|'='
name|'popen2'
op|'.'
name|'popen3'
op|'('
string|'"git-name-rev --tags `git-rev-parse %s`"'
op|'%'
name|'branch'
op|')'
newline|'\n'
DECL|variable|output
name|'output'
op|'='
name|'sout'
op|'.'
name|'read'
op|'('
op|')'
newline|'\n'
name|'if'
name|'output'
op|'.'
name|'endswith'
op|'('
string|'"\\n"'
op|')'
op|':'
newline|'\n'
DECL|variable|output
indent|'            '
name|'output'
op|'='
name|'output'
op|'['
op|':'
op|'-'
number|'1'
op|']'
newline|'\n'
DECL|variable|tagIdx
dedent|''
name|'tagIdx'
op|'='
name|'output'
op|'.'
name|'index'
op|'('
string|'" tags/p4/"'
op|')'
newline|'\n'
DECL|variable|caretIdx
name|'caretIdx'
op|'='
name|'output'
op|'.'
name|'find'
op|'('
string|'"^"'
op|')'
newline|'\n'
DECL|variable|endPos
name|'endPos'
op|'='
name|'len'
op|'('
name|'output'
op|')'
newline|'\n'
name|'if'
name|'caretIdx'
op|'!='
op|'-'
number|'1'
op|':'
newline|'\n'
DECL|variable|endPos
indent|'            '
name|'endPos'
op|'='
name|'caretIdx'
newline|'\n'
DECL|variable|rev
dedent|''
name|'rev'
op|'='
name|'int'
op|'('
name|'output'
op|'['
name|'tagIdx'
op|'+'
number|'9'
op|':'
name|'endPos'
op|']'
op|')'
op|'+'
number|'1'
newline|'\n'
DECL|variable|changeRange
name|'changeRange'
op|'='
string|'"@%s,#head"'
op|'%'
name|'rev'
newline|'\n'
DECL|variable|initialParent
name|'initialParent'
op|'='
name|'os'
op|'.'
name|'popen'
op|'('
string|'"git-rev-parse %s"'
op|'%'
name|'branch'
op|')'
op|'.'
name|'read'
op|'('
op|')'
op|'['
op|':'
op|'-'
number|'1'
op|']'
newline|'\n'
DECL|variable|initialTag
name|'initialTag'
op|'='
string|'"p4/%s"'
op|'%'
op|'('
name|'int'
op|'('
name|'rev'
op|')'
op|'-'
number|'1'
op|')'
newline|'\n'
dedent|''
name|'except'
op|':'
newline|'\n'
indent|'        '
name|'pass'
newline|'\n'
nl|'\n'
dedent|''
dedent|''
name|'sys'
op|'.'
name|'stderr'
op|'.'
name|'write'
op|'('
string|'"\\n"'
op|')'
newline|'\n'
nl|'\n'
DECL|variable|tz
name|'tz'
op|'='
op|'-'
name|'time'
op|'.'
name|'timezone'
op|'/'
number|'36'
newline|'\n'
DECL|variable|tzsign
name|'tzsign'
op|'='
op|'('
string|'"%s"'
op|'%'
name|'tz'
op|')'
op|'['
number|'0'
op|']'
newline|'\n'
name|'if'
name|'tzsign'
op|'!='
string|"'+'"
name|'and'
name|'tzsign'
op|'!='
string|"'-'"
op|':'
newline|'\n'
DECL|variable|tz
indent|'    '
name|'tz'
op|'='
string|'"+"'
op|'+'
op|'('
string|'"%s"'
op|'%'
name|'tz'
op|')'
newline|'\n'
nl|'\n'
dedent|''
name|'gitOutput'
op|','
name|'gitStream'
op|','
name|'gitError'
op|'='
name|'popen2'
op|'.'
name|'popen3'
op|'('
string|'"git-fast-import"'
op|')'
newline|'\n'
nl|'\n'
name|'if'
name|'len'
op|'('
name|'revision'
op|')'
op|'>'
number|'0'
op|':'
newline|'\n'
indent|'    '
name|'print'
string|'"Doing initial import of %s from revision %s"'
op|'%'
op|'('
name|'globalPrefix'
op|','
name|'revision'
op|')'
newline|'\n'
nl|'\n'
DECL|variable|details
name|'details'
op|'='
op|'{'
string|'"user"'
op|':'
string|'"git perforce import user"'
op|','
string|'"time"'
op|':'
name|'int'
op|'('
name|'time'
op|'.'
name|'time'
op|'('
op|')'
op|')'
op|'}'
newline|'\n'
name|'details'
op|'['
string|'"desc"'
op|']'
op|'='
string|'"Initial import of %s from the state at revision %s"'
op|'%'
op|'('
name|'globalPrefix'
op|','
name|'revision'
op|')'
newline|'\n'
name|'details'
op|'['
string|'"change"'
op|']'
op|'='
name|'revision'
newline|'\n'
DECL|variable|newestRevision
name|'newestRevision'
op|'='
number|'0'
newline|'\n'
nl|'\n'
DECL|variable|fileCnt
name|'fileCnt'
op|'='
number|'0'
newline|'\n'
name|'for'
name|'info'
name|'in'
name|'p4CmdList'
op|'('
string|'"files %s...%s"'
op|'%'
op|'('
name|'globalPrefix'
op|','
name|'revision'
op|')'
op|')'
op|':'
newline|'\n'
DECL|variable|change
indent|'        '
name|'change'
op|'='
name|'int'
op|'('
name|'info'
op|'['
string|'"change"'
op|']'
op|')'
newline|'\n'
name|'if'
name|'change'
op|'>'
name|'newestRevision'
op|':'
newline|'\n'
DECL|variable|newestRevision
indent|'            '
name|'newestRevision'
op|'='
name|'change'
newline|'\n'
nl|'\n'
dedent|''
name|'if'
name|'info'
op|'['
string|'"action"'
op|']'
op|'=='
string|'"delete"'
op|':'
newline|'\n'
indent|'            '
name|'continue'
newline|'\n'
nl|'\n'
dedent|''
name|'for'
name|'prop'
name|'in'
op|'['
string|'"depotFile"'
op|','
string|'"rev"'
op|','
string|'"action"'
op|','
string|'"type"'
op|']'
op|':'
newline|'\n'
indent|'            '
name|'details'
op|'['
string|'"%s%s"'
op|'%'
op|'('
name|'prop'
op|','
name|'fileCnt'
op|')'
op|']'
op|'='
name|'info'
op|'['
name|'prop'
op|']'
newline|'\n'
nl|'\n'
DECL|variable|fileCnt
dedent|''
name|'fileCnt'
op|'='
name|'fileCnt'
op|'+'
number|'1'
newline|'\n'
nl|'\n'
dedent|''
name|'details'
op|'['
string|'"change"'
op|']'
op|'='
name|'newestRevision'
newline|'\n'
nl|'\n'
name|'try'
op|':'
newline|'\n'
indent|'        '
name|'commit'
op|'('
name|'details'
op|','
name|'extractFilesFromCommit'
op|'('
name|'details'
op|')'
op|','
name|'branch'
op|','
name|'globalPrefix'
op|')'
newline|'\n'
dedent|''
name|'except'
op|':'
newline|'\n'
indent|'        '
name|'print'
name|'gitError'
op|'.'
name|'read'
op|'('
op|')'
newline|'\n'
nl|'\n'
dedent|''
dedent|''
name|'else'
op|':'
newline|'\n'
DECL|variable|changes
indent|'    '
name|'changes'
op|'='
op|'['
op|']'
newline|'\n'
nl|'\n'
name|'if'
name|'len'
op|'('
name|'changesFile'
op|')'
op|'>'
number|'0'
op|':'
newline|'\n'
DECL|variable|output
indent|'        '
name|'output'
op|'='
name|'open'
op|'('
name|'changesFile'
op|')'
op|'.'
name|'readlines'
op|'('
op|')'
newline|'\n'
DECL|variable|changeSet
name|'changeSet'
op|'='
name|'Set'
op|'('
op|')'
newline|'\n'
name|'for'
name|'line'
name|'in'
name|'output'
op|':'
newline|'\n'
indent|'            '
name|'changeSet'
op|'.'
name|'add'
op|'('
name|'int'
op|'('
name|'line'
op|')'
op|')'
newline|'\n'
nl|'\n'
dedent|''
name|'for'
name|'change'
name|'in'
name|'changeSet'
op|':'
newline|'\n'
indent|'            '
name|'changes'
op|'.'
name|'append'
op|'('
name|'change'
op|')'
newline|'\n'
nl|'\n'
dedent|''
name|'changes'
op|'.'
name|'sort'
op|'('
op|')'
newline|'\n'
dedent|''
name|'else'
op|':'
newline|'\n'
DECL|variable|output
indent|'        '
name|'output'
op|'='
name|'os'
op|'.'
name|'popen'
op|'('
string|'"p4 changes %s...%s"'
op|'%'
op|'('
name|'globalPrefix'
op|','
name|'changeRange'
op|')'
op|')'
op|'.'
name|'readlines'
op|'('
op|')'
newline|'\n'
nl|'\n'
name|'for'
name|'line'
name|'in'
name|'output'
op|':'
newline|'\n'
DECL|variable|changeNum
indent|'            '
name|'changeNum'
op|'='
name|'line'
op|'.'
name|'split'
op|'('
string|'" "'
op|')'
op|'['
number|'1'
op|']'
newline|'\n'
name|'changes'
op|'.'
name|'append'
op|'('
name|'changeNum'
op|')'
newline|'\n'
nl|'\n'
dedent|''
name|'changes'
op|'.'
name|'reverse'
op|'('
op|')'
newline|'\n'
nl|'\n'
dedent|''
name|'if'
name|'len'
op|'('
name|'changes'
op|')'
op|'=='
number|'0'
op|':'
newline|'\n'
indent|'        '
name|'if'
name|'not'
name|'silent'
op|':'
newline|'\n'
indent|'            '
name|'print'
string|'"no changes to import!"'
newline|'\n'
dedent|''
name|'sys'
op|'.'
name|'exit'
op|'('
number|'1'
op|')'
newline|'\n'
nl|'\n'
DECL|variable|cnt
dedent|''
name|'cnt'
op|'='
number|'1'
newline|'\n'
name|'for'
name|'change'
name|'in'
name|'changes'
op|':'
newline|'\n'
DECL|variable|description
indent|'        '
name|'description'
op|'='
name|'p4Cmd'
op|'('
string|'"describe %s"'
op|'%'
name|'change'
op|')'
newline|'\n'
nl|'\n'
name|'if'
name|'not'
name|'silent'
op|':'
newline|'\n'
indent|'            '
name|'sys'
op|'.'
name|'stdout'
op|'.'
name|'write'
op|'('
string|'"\\rimporting revision %s (%s%%)"'
op|'%'
op|'('
name|'change'
op|','
name|'cnt'
op|'*'
number|'100'
op|'/'
name|'len'
op|'('
name|'changes'
op|')'
op|')'
op|')'
newline|'\n'
name|'sys'
op|'.'
name|'stdout'
op|'.'
name|'flush'
op|'('
op|')'
newline|'\n'
DECL|variable|cnt
dedent|''
name|'cnt'
op|'='
name|'cnt'
op|'+'
number|'1'
newline|'\n'
nl|'\n'
comment|'#        try:'
nl|'\n'
DECL|variable|files
name|'files'
op|'='
name|'extractFilesFromCommit'
op|'('
name|'description'
op|')'
newline|'\n'
name|'if'
name|'detectBranches'
op|':'
newline|'\n'
indent|'            '
name|'for'
name|'branch'
name|'in'
name|'branchesForCommit'
op|'('
name|'files'
op|')'
op|':'
newline|'\n'
indent|'                '
name|'knownBranches'
op|'.'
name|'add'
op|'('
name|'branch'
op|')'
newline|'\n'
DECL|variable|branchPrefix
name|'branchPrefix'
op|'='
name|'globalPrefix'
op|'+'
name|'branch'
op|'+'
string|'"/"'
newline|'\n'
DECL|variable|branch
name|'branch'
op|'='
string|'"refs/heads/"'
op|'+'
name|'branch'
newline|'\n'
name|'commit'
op|'('
name|'description'
op|','
name|'files'
op|','
name|'branch'
op|','
name|'branchPrefix'
op|')'
newline|'\n'
dedent|''
dedent|''
name|'else'
op|':'
newline|'\n'
indent|'            '
name|'commit'
op|'('
name|'description'
op|','
name|'files'
op|','
name|'branch'
op|','
name|'globalPrefix'
op|')'
newline|'\n'
comment|'#        except:'
nl|'\n'
comment|'#            print gitError.read()'
nl|'\n'
comment|'#            sys.exit(1)'
nl|'\n'
nl|'\n'
dedent|''
dedent|''
dedent|''
name|'if'
name|'not'
name|'silent'
op|':'
newline|'\n'
indent|'    '
name|'print'
string|'""'
newline|'\n'
nl|'\n'
dedent|''
name|'gitStream'
op|'.'
name|'write'
op|'('
string|'"reset refs/tags/p4/%s\\n"'
op|'%'
name|'lastChange'
op|')'
newline|'\n'
name|'gitStream'
op|'.'
name|'write'
op|'('
string|'"from %s\\n\\n"'
op|'%'
name|'branch'
op|')'
op|';'
newline|'\n'
nl|'\n'
nl|'\n'
name|'gitStream'
op|'.'
name|'close'
op|'('
op|')'
newline|'\n'
name|'gitOutput'
op|'.'
name|'close'
op|'('
op|')'
newline|'\n'
name|'gitError'
op|'.'
name|'close'
op|'('
op|')'
newline|'\n'
nl|'\n'
name|'os'
op|'.'
name|'popen'
op|'('
string|'"git-repo-config p4.depotpath %s"'
op|'%'
name|'globalPrefix'
op|')'
op|'.'
name|'read'
op|'('
op|')'
newline|'\n'
name|'if'
name|'len'
op|'('
name|'initialTag'
op|')'
op|'>'
number|'0'
op|':'
newline|'\n'
indent|'    '
name|'os'
op|'.'
name|'popen'
op|'('
string|'"git tag -d %s"'
op|'%'
name|'initialTag'
op|')'
op|'.'
name|'read'
op|'('
op|')'
newline|'\n'
nl|'\n'
dedent|''
name|'sys'
op|'.'
name|'exit'
op|'('
number|'0'
op|')'
newline|'\n'
endmarker|''
end_unit
