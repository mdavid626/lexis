/' Zajisteni zakladni kompatibility IFJ10->FreeBASIC '/
#Lang "deprecated" /' Starsi dialekt BASICu '/
Option Escape /' Aktivuje escape sekvence '/

Function sort (s As String) As String
/' Bubble Sort '/
Dim i, j, n, finished As Integer
Dim _swap As Byte
Dim result As String
  result = s
  n = Len(result)
  i = 1
  Do
    finished = 1
    For j = n - 1 To i Step -1
      If result[j-1] > result[j] Then
        _swap = result[j-1]
        result[j-1] = result[j]
        result[j] = _swap
        finished = 0
      End If
    Next j
    i += 1
  Loop Until (finished or (i = n + 1))
  Return result
End Function

Function find (haystack As String, needle As String) As Integer
  Return InStr(haystack, needle)
End Function

/' Program 23: Zakladni operace '/

Scope
dim a as integer
dim b as integer
dim c as integer

dim a_d as double
dim b_d as double
dim c_d as double
dim result as double

dim a_s as string
dim b_s as string
dim c_s as string

/' Scitani integeru '/
a = 0
print a 				;            '0
b = 2
print b 				;            '2
c = a + b
print c				;            '2
c = 10 + 20
print c				;            '30
c = c + 20
print c				;            '50
c = 20 + c
print c				;            '70
print "\n"			;

/' Odcitani integeru '/
a = 0
print a				;            '0
b = 2
print b				;            '2
c = a - b
print c				;            '-2
c = 10 - 20
print c				;            '-10
c = c - 20
print c				;            '-30
c = 20 - c
print c				;            '50
print "\n"			;

/' Nasobeni integeru '/
a = 0
print a				;            '0
b = 2
print b				;            '2
c = a * b
print c				;            '0
c = 10 * 20
print c				;            '200
c = c * 20
print c				;            '4000
c = 20*c
print c				;            '80000
print "\n"			;

/' Deleni integeru '/
a = 0
print a				;            '0
b = 2
print b				;            '2
c = a \ b
print c				;            '0
c = 30 \ 20
c = c*101
print c				;            '1
c = c \ 20
print c				;            '5
c = 20 \ c
print c				;            '4
print "\n"			;







/' konkatenace retezcu '/
print b_s				;            '
c_s = A_S + B_S
print c_s				;            '
b_s = b_s + "abcd"
print b_s				;            'abcd
b_s = "efgh" + "ijkl"
print b_s				;            'efghijkl
b_s = "ijkl" + ""
print b_s				;            'ijkl
b_s = "ijkl" + b_s		
print b_s				;            'ijklijkl
print "\n"				;


' DOUBLY
a_d = 132.0
b_d = 25.0
c_d = 42.0

result = (((a_d * b_d + 22.0 ) * (c_d/2.0 + b_d) / (c_d - b_d)))
print result;
print "\n";



end scope
