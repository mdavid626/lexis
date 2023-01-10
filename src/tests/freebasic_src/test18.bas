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

/' Program 18: Hard functions '/

function power(a as double, n as integer) as double
dim actual as integer
dim result as double
result = 1
actual = 0

do while (((((actual < n)))))
	result = result * a
	actual = actual + 1
loop

return result
end function

function max( a as double, b as double) as double
	if (a > b) then
		return a
	else
		return b
	end if
end function



scope
dim a as integer
dim b as double
dim result as double

a = 2
b = 1

result = ((max(power(2.0, 10), max(6.0, 3.0)) * 2 * power(max(2.0, 1.0), 2)))

print result; "\n";


end scope
