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

/' Program 24: Zavorky '/

scope
  dim a as integer
  dim b as integer
  dim c as integer 
  dim d as integer 
  
  c = 101
  d = 202
  if ((c*((d+c))) = (((c+d) *c))) then
    print "ok\n";
  else 
    print "fail\n";
  end if

  if ((((((((((((( ( ((((d)))) \ (((((c))))) ) \ ( (((((((d))))))) \ ((((((((((c)))))))))) ) ))) > 0)))))))))) then
   print "ok\n";
  else
    print "fail\n";
  end if

end scope

