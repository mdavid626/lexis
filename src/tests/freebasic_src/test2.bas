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

/' Program 2: Vypocet faktorialu (rekurzivne) '/

Declare Function factorial (n As Integer) As Integer
Function factorial (n As Integer) As Integer
  Dim temp_result As Integer
  Dim decremented_n As Integer
  Dim result As Integer
  If n < 2 Then
    result = 1
  Else
    decremented_n = n - 1
    temp_result = factorial(decremented_n)
    result = n * temp_result
  End If
  Return result
End Function

Scope 'Hlavni telo programu
  Dim a As Integer
  Dim vysl As Integer
  
  Print "Zadejte cislo pro vypocet faktorialu";
  Input ; a
  If a < 0 Then
    Print "\nFaktorial nelze spocitat\n";
  Else
    vysl = factorial(a)
    Print "\nVysledek je:" ; vysl ; "\n";
  End If
End Scope
