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

/' Program 14: full function '/

function something (n as integer ) as integer
dim a as integer

a = n

print a;

end function

scope
end scope
