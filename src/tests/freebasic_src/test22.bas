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

/' Program 22: Advanced if '/

scope
dim i1 as integer
dim i2 as integer
dim f1 as double
dim f2 as double
dim s1 as string
dim s2 as string

i1 = 5
i2 = 6
/' porovnani integeru '/
if i1 < i2 then
	print 1;	' 1
else
	print 0;
end if
if i1 <= i2 then
	print 1;	' 1
else
	print 0;
end if
if i1 > i2 then
	print 0;
else
	print 1;	' 1 
end if
i2 = 5
if i1 >= i2 then
	print 1;	' 1 
else
	print 0;	 
end if
if i1 = i2 then
	print 1;	' 1 
else
	print 0;	 
end if
if i1 <> i2 then
	print 0;
else
	print 1;	' 1	 
end if
print "\n";

/' porovnani doublu '/
f1 = 4.5
f2 = 4.9
if f1 < f2 then
	print 1;	' 1
else
	print 0;
end if
if f1 <= f2 then
	print 1;	' 1
else
	print 0;
end if
if f1 > f2 then
	print 0;
else
	print 1;	' 1 
end if
f2 = 4.5
if f1 >= f2 then
	print 1;	' 1 
else
	print 0;	 
end if
if f1 = f2 then
	print 1;	' 1 
else
	print 0;	 
end if
if f1 <> f2 then
	print 0;
else
	print 1;	' 1	 
end if


print "\n";
/' porovnani stringu '/
s1 = "aa"
s2 = "ab"

if s1 < s2 then
	print 1;	' 1
else
	print 0;
end if
if s1 <= s2 then
	print 1;	' 1
else
	print 0;
end if
if s1 > s2 then
	print 0;
else
	print 1;	' 1 
end if
s2 = "aa"
if s1 >= s2 then
	print 1;	' 1 
else
	print 0;	 
end if
if s1 = s2 then
	print 1;	' 1 
else
	print 0;	 
end if
if s1 <> s2 then
	print 0;
else
	print 1;	' 1	 
end if
print "\n";

i1 = s1 = s2
print i1;
print s1 <> s2;
print "\n";
end scope
