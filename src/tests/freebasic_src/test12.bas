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

/' Program 12: fibonacci cisla '/

function fibonacci(n as integer) as integer
  if ( n <= 1 ) then 
    if ( n = 1 ) then
      return 1
    else
      return 0
    end if
  else
    return ((fibonacci(n-1) + fibonacci(n-2)))
  end if
end function

scope
dim n as integer
n = 0

do while n < 20
	print fibonacci(n);
	n = n + 1
loop
print "\n";
end scope
