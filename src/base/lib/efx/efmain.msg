/*
error messages of efx Library

$Copyright (C) 1994 Erich Frühstück
This file is part of EFEU.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; see the file COPYING.Library.
If not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
*/

1	$!: MESSAGE $1.\n
2	$!: ERROR $1.\n
3	$!: format file $1 not found.\n

5	$!: assignment table is not defined.\n
6	$!: configuration file $1 not found.\n
8	$!: $0: file $1 not found.\n

21	$!: file $1 not readable.\n
22	$!: file $1 not writeable.\n
23	$!: file $1, access $2 not possible.\n
24	$!: closing a file not registered by filenotice().\n
25	$!: use of a closed IO-structure.\n
26	$!: could not create temporary file.\n

28	$!: undefiined access.\n
29	$!: process $1 not created.\n

31	$!: option -$1 - missing argument.\n
32	$!: unknown option -$1.\n
33	$!: resource file $1 created.\n
34	$!: file $1 created.\n
35	$!: database index out of bound.\n
38	$!: variable $1 ist not defined.\n
39	$!: could not initialize variable $1 (wrong type).\n

41	$!: can not expand const data buffuer.\n
42	$!: file $0: unexpected EOF by reading data field.\n
43	$!: file $0: output error by writing data field.\n

60	$!: function $"
61	$": argument $1 not defined.\n
62	$": argument $1 is not an L-value.\n
63	$": too many arguments.\n

65	$!: EvalFunc() called with invalid pointer!\n

70	$!: operator $1 not found.\n
71	$!: function $1 not found.\n
72	$!: $0: unexpected character $1 in command line.\n
73	$!: file $1 (path $2) not found.\n
74	$!: funktion $1 for type $2 not found.\n
75	$!: $1-constructor for given arguments not defined.\n

81	$!: $0: missing name of function/variable.\n
82	$!: $0: missing return type of function.\n
83	$!: $0: unknown type $1.\n
84	$!: $0: extra character after ellipsis $1.\n
85	$!: $0: syntax error in ellipsis $1.\n
86	$!: default value for L-value not permitted.\n
87	$!: missing arguments.\n
88	$!: argument $1 is not a L-value.\n
89	$!: $0: missing object type.\n
90	$!: $0: syntax error in data type $1.\n

211	$!: left side of assignment is not an object.\n
212	$!: right side of assignment is not an object.\n
213	$!: Left object of assignment is not an L-value.\n
214	$!: argument of operator $1 is not defined.\n
215	$!: operator $1 for type $2 is not defined.\n
216	$!: range list: comparsion function is not defined.\n
217	$!: range list: step size is not determinable.\n

91	$!: function $1: null effect.\n
92	$!: function $1: wrong arguments.\n
93	$!: function $1: argument $2 is not defined.\n
94	$!: function $1: undefined default value for argument $2.\n
95	$!: $0: invalid name in for-in loop.\n
96	$!: $0: function $1: No argument for return reference defined.\n
97	$!: function $1: ATTENTION: incompatible return reference.\n

101	$!: $0: operator $1: right argument is missing.\n
102	$!: $0: conditional expression: ":" is missing.\n
103	$!: $0: partial term: right parenthese is missing.\n
104	$!: $0: expression: right bracket is missing.\n
105	$!: $0: missing end of string.\n
106	$!: $0: incomplete argument list.\n
108	$!: $0: missing end of charachter constant.\n
109	$!: $0: syntax error: read $1 where $2 is expected.\n
110	$!: $0: syntax error: incomplete block.\n

111	$!: function $1: null effect.\n
112	$!: function $1: wrong arguments.\n
113	$!: $0: ATTENTION: null effect.\n
114	$!: $0: unexpected character $1.\n
115	$!: $0: name of function is missing.\n
116	$!: $0: syntax error in function declaration $1.\n
117	$!: $0: command line is missing.\n
	
120	$!: object of type $1 already released!\n
121	$!: $0: unexpected end of file.\n
122	$!: $0: syntax error in structure declaration.\n
123	$!: $0: syntax error in type declaration.\n
124	$!: $0: memory size of $1 is not defined.\n
125	$!: undefined object $1 in argument list.\n
126	$!: undefined default value in function call.\n
127	$!: $0: type of function is missing.\n
128	$!: $0: undefined data type$1.\n
129	$!: $0: undefined object binding in prototype.\n

221	$!: $0: destructor with return value $1.\n
222	$!: $0: converter for $1 without return value.\n
223	$!: $0: argument for destructor/constructor not allowed.\n
224	$!: $0: extra character after void key.\n
225	$!: $0: scope with invalid data type $1.\n

131	$!: assignment: left object is not an L-value!\n
132	$!: assignment: right object is not defined!\n
133	$!: assignment: right object has incompatible type!\n
134	$!: object is not a scalar!\n
135	$!: object is not a vector!\n
136	$!: object of type $1 could not be copied!\n
137	$!: ATTENTION: recursive object of type $1!\n
138	$!: extra initialisation values are ignored.\n

141	$!: unknown type $1.\n
142	$!: extra character after ellipse $2.\n
143	$!: default value for L-value is not allowed.\n
144	$!: could not copy function block.\n
145	$!: could not copy term or list.\n
146	$!: ellipse in argument list would be ignored.\n

151	$!: could not copy table of variables.\n
152	$!: $0: syntax error in variable declaration.\n
153	$!: $0: undefined data type$1.\n
154	$!: ATTENTION: variable $1 new declared.\n
154b	$!: ATTENTION: variable $1 allready declared.\n
155	$!: vector index out of bounds.\n
255	$!: vector not initialized.\n
256	$!: immutable vector size.\n
156	$!: $0: syntax error in structure declaration.\n
157	$!: ATTENTION: new declaration of structure $1.\n
157b	$!: ATTENTION: structure $1 allready declared in a different way.\n
158	$!: allocation size of $1 is not defined.\n
159	$!: unexpected member in structure $1.\n

161	$!: could not convert $1 to $2.\n
162	$!: name $1 not found.\n
163	$!: could not read objects of type $1.\n
164	$!: object of type $1: component $2 is not defined.\n
165	$!: asignment object does not allow decomposition.\n
166	$!: variable $1 is not defined.\n
167	$!: operand of lvalue-cast is not an lvalue.\n
168	$!: changing a const declared argument.\n
169	$!: function argument $1 furthermore in use.\n

171	$!: unexpected "break".\n
172	$!: unexpected "continue".\n
173	$!: undefined function/operator $1.\n
174	$!: undefined function/operator $1.\n
175	$!: function $1: could not create lvalue.\n

181	$!: $0: EOF in list.\n
182	$!: $0: missing object before $1.\n
183	$!: $0: unexpected character $1 in list.\n
185	$!: $0: syntax error in enum declaration.\n
186	$!: $0: base class $1 must be an enum type.\n

191	$!: $0: missing member/scope name.\n
192	$!: $0: component $1 of scope is not definiert.\n
193	$!: $0: component $1 is not defined.\n
194	$!: $0: syntax error in vector index.\n
195	$!: $0: left operand of assignment is not an lvalue.\n
196	$!: $0: global operator without name.\n

201	$!: Function "$1": component $2 already in use.\n
202	$!: Function "$1": name already in use.\n
203	$!: ATTENTION: new declaration of Function "$1".\n
205	$!: Function "$1" assumpted to be virtual.\n
206	$!: Illegal Function declaration.\n

301	$!: undefined random number generator $1.\n
302	$!: undefined size in variable declaration.\n

401	$!: $0: missing block after switch-statement.\n
402	$!: $0: syntax error in case-statement.\n
403	$!: $0: unexpected character after default statement.\n
404	$!: $0: command lines befor case are ignored.\n

ppm	$!: $0: invalid header in pixmap file.\n
nocmp	$!: compare function not defined.\n

vdiff	$!: $0: data type $1: wrong version $2 != $3.\n
vskip	$!: $0: no version control for data type $1.\n
