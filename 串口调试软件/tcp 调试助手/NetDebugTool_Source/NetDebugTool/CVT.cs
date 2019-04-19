namespace NetDebugTool
{
    using System;

    public class CVT
    {
        public static string BinDotBin(string bin)
        {
            string rtn = "";
            for (int i = 0; i < bin.Length; i++)
            {
                rtn = rtn + bin.Substring(i, 1) + ",";
            }
            return rtn.TrimEnd(new char[] { ',' });
        }

        public static string BitDivision(int begin, int end, byte byt)
        {
            string str1 = "1";
            int a = Convert.ToInt32(str1.PadRight((end - begin) + 1, '1').PadLeft(end, '0').PadRight(8, '0'), 2);
            int b = byt;
            int c = a & b;
            c = c >> (8 - end);
            return Convert.ToString(c, 2).PadLeft((end - begin) + 1, '0');
        }

        public static byte[] ByteAdd(byte[] by1, byte[] by2)
        {
            int i;
            int l1 = by1.Length;
            int l2 = by2.Length;
            byte[] by = new byte[l1 + l2];
            for (i = 0; i < l1; i++)
            {
                by[i] = by1[i];
            }
            for (i = 0; i < l2; i++)
            {
                by[i + l1] = by2[i];
            }
            return by;
        }

        public static string ByteToHexStr(byte[] da)
        {
            string s = "";
            for (int i = 0; i < da.Length; i++)
            {
                s = s + Convert.ToString(da[i], 0x10).PadLeft(2, '0').ToUpper() + " ";
            }
            return s;
        }

        public static string ByteToHexStr2(byte[] da)
        {
            string s = "";
            for (int i = 0; i < da.Length; i++)
            {
                s = s + Convert.ToString(da[i], 0x10).PadLeft(2, '0');
            }
            return s;
        }

        public static string HexStrToBinStr(string hex)
        {
            int l = hex.Length * 4;
            return Convert.ToString(Convert.ToInt32(hex, 0x10), 2).PadLeft(l, '0');
        }

        public static bool IsDigit(string str)
        {
            for (int i = 0; i < str.Length; i++)
            {
                if ((str[i] < '0') || (str[i] > '9'))
                {
                    return false;
                }
            }
            return true;
        }

        public static byte[] StrToHexByte(string da)
        {
            string sends = da;
            sends = sends.Replace(" ", "").Replace("\n", "").Replace("\r", "");
            int length = sends.Length / 2;
            byte[] sendb = new byte[length];
            for (int i = 0; i < length; i++)
            {
                sendb[i] = Convert.ToByte(sends.Substring(i * 2, 2), 0x10);
            }
            return sendb;
        }

        public static string StrTOStrV(string str)
        {
            string retn = "";
            int length = str.Length;
            for (int i = 0; i <= (length - 2); i += 2)
            {
                retn = str.Substring(i, 2) + retn;
            }
            return retn;
        }

        public static byte[] SubByteArray(byte[] by, int index)
        {
            return SubByteArray(by, index, by.Length - index);
        }

        public static byte[] SubByteArray(byte[] by, int index, int length)
        {
            byte[] byr = new byte[length];
            for (int i = index; i < (index + length); i++)
            {
                byr[i - index] = by[i];
            }
            return byr;
        }

        public static string ToSix(string str)
        {
            long l = Convert.ToInt64(str, 2);
            return ("0x" + Convert.ToString(l, 0x10).PadLeft(2, '0'));
        }

        public static string ToTen(string str)
        {
            return Convert.ToString(Convert.ToInt64(str, 2), 10);
        }
    }
}

