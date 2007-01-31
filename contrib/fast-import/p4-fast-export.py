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
comment|'#       - support incremental imports'
nl|'\n'
comment|'#       - create tags'
nl|'\n'
comment|'#       - instead of reading all files into a variable try to pipe from'
nl|'\n'
comment|'#       - support p4 submit (hah!)'
nl|'\n'
comment|"#       - don't hardcode the import to master"
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
newline|'\n'
nl|'\n'
name|'if'
name|'len'
op|'('
name|'sys'
op|'.'
name|'argv'
op|')'
op|'!='
number|'2'
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
string|'"    %s //depot/my/project/ -- to import everything"'
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
nl|'\n'
DECL|variable|prefix
dedent|''
name|'prefix'
op|'='
name|'sys'
op|'.'
name|'argv'
op|'['
number|'1'
op|']'
newline|'\n'
DECL|variable|changeRange
name|'changeRange'
op|'='
string|'""'
newline|'\n'
name|'try'
op|':'
newline|'\n'
DECL|variable|atIdx
indent|'    '
name|'atIdx'
op|'='
name|'prefix'
op|'.'
name|'index'
op|'('
string|'"@"'
op|')'
newline|'\n'
DECL|variable|changeRange
name|'changeRange'
op|'='
name|'prefix'
op|'['
name|'atIdx'
op|':'
op|']'
newline|'\n'
DECL|variable|prefix
name|'prefix'
op|'='
name|'prefix'
op|'['
number|'0'
op|':'
name|'atIdx'
op|']'
newline|'\n'
dedent|''
name|'except'
name|'ValueError'
op|':'
newline|'\n'
DECL|variable|changeRange
indent|'    '
name|'changeRange'
op|'='
string|'""'
newline|'\n'
nl|'\n'
dedent|''
name|'if'
name|'prefix'
op|'.'
name|'endswith'
op|'('
string|'"..."'
op|')'
op|':'
newline|'\n'
DECL|variable|prefix
indent|'    '
name|'prefix'
op|'='
name|'prefix'
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
name|'prefix'
op|'.'
name|'endswith'
op|'('
string|'"/"'
op|')'
op|':'
newline|'\n'
indent|'    '
name|'prefix'
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
DECL|variable|output
name|'output'
op|'='
name|'os'
op|'.'
name|'popen'
op|'('
string|'"p4 changes %s...%s"'
op|'%'
op|'('
name|'prefix'
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
DECL|variable|changes
name|'changes'
op|'='
op|'['
op|']'
newline|'\n'
name|'for'
name|'line'
name|'in'
name|'output'
op|':'
newline|'\n'
DECL|variable|changeNum
indent|'    '
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
nl|'\n'
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
DECL|variable|cnt
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
indent|'    '
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
name|'cnt'
op|'='
name|'cnt'
op|'+'
number|'1'
newline|'\n'
nl|'\n'
DECL|variable|epoch
name|'epoch'
op|'='
name|'description'
op|'['
string|'"time"'
op|']'
newline|'\n'
DECL|variable|author
name|'author'
op|'='
name|'description'
op|'['
string|'"user"'
op|']'
newline|'\n'
nl|'\n'
name|'gitStream'
op|'.'
name|'write'
op|'('
string|'"commit refs/heads/master\\n"'
op|')'
newline|'\n'
DECL|variable|committer
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
DECL|variable|committer
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
DECL|variable|committer
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
name|'description'
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
name|'prefix'
op|','
name|'change'
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
DECL|variable|fnum
name|'fnum'
op|'='
number|'0'
newline|'\n'
name|'while'
name|'description'
op|'.'
name|'has_key'
op|'('
string|'"depotFile%s"'
op|'%'
name|'fnum'
op|')'
op|':'
newline|'\n'
DECL|variable|path
indent|'        '
name|'path'
op|'='
name|'description'
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
name|'prefix'
op|')'
op|':'
newline|'\n'
indent|'            '
name|'print'
string|'"\\nchanged files: ignoring path %s outside of %s in change %s"'
op|'%'
op|'('
name|'path'
op|','
name|'prefix'
op|','
name|'change'
op|')'
newline|'\n'
DECL|variable|fnum
name|'fnum'
op|'='
name|'fnum'
op|'+'
number|'1'
newline|'\n'
name|'continue'
newline|'\n'
nl|'\n'
DECL|variable|rev
dedent|''
name|'rev'
op|'='
name|'description'
op|'['
string|'"rev%s"'
op|'%'
name|'fnum'
op|']'
newline|'\n'
DECL|variable|depotPath
name|'depotPath'
op|'='
name|'path'
op|'+'
string|'"#"'
op|'+'
name|'rev'
newline|'\n'
DECL|variable|relPath
name|'relPath'
op|'='
name|'path'
op|'['
name|'len'
op|'('
name|'prefix'
op|')'
op|':'
op|']'
newline|'\n'
DECL|variable|action
name|'action'
op|'='
name|'description'
op|'['
string|'"action%s"'
op|'%'
name|'fnum'
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
DECL|variable|mode
indent|'            '
name|'mode'
op|'='
number|'644'
newline|'\n'
name|'if'
name|'description'
op|'['
string|'"type%s"'
op|'%'
name|'fnum'
op|']'
op|'.'
name|'startswith'
op|'('
string|'"x"'
op|')'
op|':'
newline|'\n'
DECL|variable|mode
indent|'                '
name|'mode'
op|'='
number|'755'
newline|'\n'
nl|'\n'
DECL|variable|data
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
DECL|variable|fnum
dedent|''
name|'fnum'
op|'='
name|'fnum'
op|'+'
number|'1'
newline|'\n'
nl|'\n'
dedent|''
name|'gitStream'
op|'.'
name|'write'
op|'('
string|'"\\n"'
op|')'
newline|'\n'
nl|'\n'
name|'gitStream'
op|'.'
name|'write'
op|'('
string|'"tag p4/%s\\n"'
op|'%'
name|'change'
op|')'
newline|'\n'
name|'gitStream'
op|'.'
name|'write'
op|'('
string|'"from refs/heads/master\\n"'
op|')'
op|';'
newline|'\n'
name|'gitStream'
op|'.'
name|'write'
op|'('
string|'"tagger %s\\n"'
op|'%'
name|'committer'
op|')'
op|';'
newline|'\n'
name|'gitStream'
op|'.'
name|'write'
op|'('
string|'"data 0\\n\\n"'
op|')'
newline|'\n'
nl|'\n'
nl|'\n'
dedent|''
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
name|'print'
string|'""'
newline|'\n'
endmarker|''
end_unit
