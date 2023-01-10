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

/' Program 3: Prace s retezci a vestavenymi funkcemi '/

Scope 'Hlavni telo programu
  Dim str1 As String
  Dim str2 As String
  Dim p As Integer
  
  str1 = "Toto je nejaky text"
  str2 = str1 + ", ktery jeste trochu obohatime"
  Print str1; "\n"; str2; "\n";
  p = find(str2, "text")
  Print "Pozice retezce \"text\" v retezci str2:"; p; "\n";
  Print "Zadejte nejakou posloupnost vsech malych pismen a-h, ";
  Print "pricemz se pismena nesmeji v posloupnosti opakovat";
  Input ; str1
  str2 = sort(str1)
  Do While (str2 <> "abcdefgh")
    Print "\nSpatne zadana posloupnost, zkuste znovu";
    Input ; str1
    str2 = sort(str1)
  Loop
  Print "\n";
End Scope
