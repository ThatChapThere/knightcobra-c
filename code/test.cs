using System;

public static class Kata
{
  public static bool IsPangram(string str)
  {
    //throw new NotImplementedException();
    
    char[] letters = str.ToLower().ToCharArray();
    
    bool a = false, b = false, c = false, d = false, e = false, f = false, g = false, h = false, i = false, j = false, k = false, l = false, m = false, n = false, o = false, p = false, q = false, r = false, s = false, t = false, u = false, v = false, w = false, x = false, y = false, z = false;
    
    for (int number = 0; number < letters.Length; number++)
    {
      switch (letters[number])
      {
        case 'a':
          a = true;
          break;
        case 'b':
          b = true;
          break;
        case 'c':
          c = true;
          break;
        case 'd':
          d = true;
          break;
        case 'e':
          e = true;
          break;
        case 'f':
          f = true;
          break;
        case 'g':
          g = true;
          break;
        case 'h':
          h = true;
          break;
        case 'i':
          i = true;
          break;
        case 'j':
          j = true;
          break;
        case 'k':
          k = true;
          break;
        case 'l':
          l = true;
          break;
        case 'm':
          m = true;
          break;
        case 'n':
          n = true;
          break;
        case 'o':
          o = true;
          break;
        case 'p':
          p = true;
          break;
        case 'q':
          q = true;
          break;
        case 'r':
          r = true;
          break;
        case 's':
          s = true;
          break;
        case 't':
          t = true;
          break;
        case 'u':
          u = true;
          break;
        case 'v':
          v = true;
          break;
        case 'w':
          w = true;
          break;
        case 'x':
          x = true;
          break;
        case 'y':
          y = true;
          break;
        case 'z':
          z = true;
          break;
        default:
          break;
      }
    }
    if (a == true && b == true && c == true && d == true && e == true && f == true && g == true && h == true && i == true && j == true && k == true && l == true && m == true && n == true && o == true && p == true && q == true && r == true && s == true && t == true && u == true && v == true && w == true && x == true && y == true && z == true)
    {
      return true;
    }
    else
    {
      return false;
    }
  }
}