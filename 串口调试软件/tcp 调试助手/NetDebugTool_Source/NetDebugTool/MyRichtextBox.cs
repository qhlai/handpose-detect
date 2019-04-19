namespace NetDebugTool
{
    using System;
    using System.Collections.Specialized;
    using System.Drawing;
    using System.Drawing.Imaging;
    using System.IO;
    using System.Runtime.InteropServices;
    using System.Text;
    using System.Windows.Forms;

    public class MyRichtextBox : RichTextBox
    {
        private const string FF_UNKNOWN = "UNKNOWN";
        private Color highlightColor;
        private const int HMM_PER_INCH = 0x9ec;
        private const int MM_ANISOTROPIC = 8;
        private const int MM_HIENGLISH = 5;
        private const int MM_HIMETRIC = 3;
        private const int MM_ISOTROPIC = 7;
        private const int MM_LOENGLISH = 4;
        private const int MM_LOMETRIC = 2;
        private const int MM_TEXT = 1;
        private const int MM_TWIPS = 6;
        private const string RTF_DOCUMENT_POST = @"\cf0\fs17}";
        private const string RTF_DOCUMENT_PRE = @"\viewkind4\uc1\pard\cf1\f0\fs20";
        private const string RTF_HEADER = @"{\rtf1\ansi\ansicpg1252\deff0\deflang1033";
        private string RTF_IMAGE_POST;
        private HybridDictionary rtfFontFamily;
        private Color textColor;
        private const int TWIPS_PER_INCH = 0x5a0;
        private float xDpi;
        private float yDpi;

        public MyRichtextBox()
        {
            this.RTF_IMAGE_POST = "}";
            this.textColor = Color.Black;
            this.highlightColor = Color.White;
            this.rtfFontFamily = new HybridDictionary();
            this.rtfFontFamily.Add(FontFamily.GenericMonospace.Name, @"\fmodern");
            this.rtfFontFamily.Add(FontFamily.GenericSansSerif, @"\fswiss");
            this.rtfFontFamily.Add(FontFamily.GenericSerif, @"\froman");
            this.rtfFontFamily.Add("UNKNOWN", @"\fnil");
            using (Graphics _graphics = base.CreateGraphics())
            {
                this.xDpi = _graphics.DpiX;
                this.yDpi = _graphics.DpiY;
            }
        }

        public MyRichtextBox(Color _textColor) : this()
        {
            this.textColor = _textColor;
        }

        public MyRichtextBox(Color _textColor, Color _highlightColor) : this()
        {
            this.textColor = _textColor;
            this.highlightColor = _highlightColor;
        }

        public void AppendRtf(string _rtf)
        {
            base.Select(this.TextLength, 0);
            base.SelectedRtf = _rtf;
        }

        public void AppendTextAsRtf(string _text)
        {
            this.AppendTextAsRtf(_text, this.Font);
        }

        public void AppendTextAsRtf(string _text, Font _font)
        {
            this.AppendTextAsRtf(_text, _font, this.textColor);
        }

        public void AppendTextAsRtf(string _text, Font _font, Color _textColor)
        {
            _text = _text.Replace(@"\", @"\\");
            this.AppendTextAsRtf(_text, _font, _textColor, this.highlightColor);
        }

        public void AppendTextAsRtf(string _text, Font _font, Color _textColor, Color _backColor)
        {
            base.Select(this.TextLength, 0);
            this.InsertTextAsRtf(_text, _font, _textColor, _backColor);
        }

        [DllImport("gdiplus.dll")]
        private static extern uint GdipEmfToWmfBits(IntPtr _hEmf, uint _bufferSize, byte[] _buffer, int _mappingMode, EmfToWmfBitsFlags _flags);
        private string GetColorTable(Color _textColor, Color _backColor)
        {
            StringBuilder _colorTable = new StringBuilder();
            _colorTable.Append(@"{\colortbl ;");
            _colorTable.Append(string.Format(@"\red{0}\green{1}\blue{2}", _textColor.R.ToString(), _textColor.G.ToString(), _textColor.B.ToString()));
            _colorTable.Append(";");
            _colorTable.Append(string.Format(@"\red{0}\green{1}\blue{2}", _backColor.R.ToString(), _backColor.G.ToString(), _backColor.B.ToString()));
            _colorTable.Append(@";}\n");
            return _colorTable.ToString();
        }

        private string GetDocumentArea(string _text, Font _font)
        {
            StringBuilder _doc = new StringBuilder();
            _doc.Append(@"\viewkind4\uc1\pard\cf1\f0\fs20");
            _doc.Append(@"\highlight2");
            if (_font.Bold)
            {
                _doc.Append(@"\b");
            }
            if (_font.Italic)
            {
                _doc.Append(@"\i");
            }
            if (_font.Strikeout)
            {
                _doc.Append(@"\strike");
            }
            if (_font.Underline)
            {
                _doc.Append(@"\ul");
            }
            _doc.Append(@"\f0");
            _doc.Append(@"\fs");
            _doc.Append((int) Math.Round((double) (2f * _font.SizeInPoints)));
            _doc.Append(" ");
            _doc.Append(_text.Replace("\n", @"\par "));
            _doc.Append(@"\highlight0");
            if (_font.Bold)
            {
                _doc.Append(@"\b0");
            }
            if (_font.Italic)
            {
                _doc.Append(@"\i0");
            }
            if (_font.Strikeout)
            {
                _doc.Append(@"\strike0");
            }
            if (_font.Underline)
            {
                _doc.Append(@"\ulnone");
            }
            _doc.Append(@"\f0");
            _doc.Append(@"\fs20");
            _doc.Append(@"\cf0\fs17}");
            return _doc.ToString();
        }

        private string GetFontTable(Font _font)
        {
            StringBuilder _fontTable = new StringBuilder();
            _fontTable.Append(@"{\fonttbl{\f0");
            _fontTable.Append(@"\");
            if (this.rtfFontFamily.Contains(_font.FontFamily.Name))
            {
                _fontTable.Append(this.rtfFontFamily[_font.FontFamily.Name]);
            }
            else
            {
                _fontTable.Append(this.rtfFontFamily["UNKNOWN"]);
            }
            _fontTable.Append(@"\fcharset134 ");
            _fontTable.Append(_font.Name);
            _fontTable.Append(";}}");
            return _fontTable.ToString();
        }

        private string GetImagePrefix(Image _image)
        {
            StringBuilder _rtf = new StringBuilder();
            int picw = (int) Math.Round((double) ((((float) _image.Width) / this.xDpi) * 2540f));
            int pich = (int) Math.Round((double) ((((float) _image.Height) / this.yDpi) * 2540f));
            int picwgoal = (int) Math.Round((double) ((((float) _image.Width) / this.xDpi) * 1440f));
            int pichgoal = (int) Math.Round((double) ((((float) _image.Height) / this.yDpi) * 1440f));
            _rtf.Append(@"{\pict\wmetafile8");
            _rtf.Append(@"\picw");
            _rtf.Append(picw);
            _rtf.Append(@"\pich");
            _rtf.Append(pich);
            _rtf.Append(@"\picwgoal");
            _rtf.Append(picwgoal);
            _rtf.Append(@"\pichgoal");
            _rtf.Append(pichgoal);
            _rtf.Append(" ");
            return _rtf.ToString();
        }

        private string GetRtfImage(Image _image)
        {
            StringBuilder _rtf = null;
            MemoryStream _stream = null;
            Graphics _graphics = null;
            Metafile _metaFile = null;
            string CS$1$0000;
            try
            {
                _rtf = new StringBuilder();
                _stream = new MemoryStream();
                using (_graphics = base.CreateGraphics())
                {
                    IntPtr _hdc = _graphics.GetHdc();
                    _metaFile = new Metafile(_stream, _hdc);
                    _graphics.ReleaseHdc(_hdc);
                }
                using (_graphics = Graphics.FromImage(_metaFile))
                {
                    _graphics.DrawImage(_image, new Rectangle(0, 0, _image.Width, _image.Height));
                }
                IntPtr _hEmf = _metaFile.GetHenhmetafile();
                uint _bufferSize = GdipEmfToWmfBits(_hEmf, 0, null, 8, EmfToWmfBitsFlags.EmfToWmfBitsFlagsDefault);
                byte[] _buffer = new byte[_bufferSize];
                uint _convertedSize = GdipEmfToWmfBits(_hEmf, _bufferSize, _buffer, 8, EmfToWmfBitsFlags.EmfToWmfBitsFlagsDefault);
                for (int i = 0; i < _buffer.Length; i++)
                {
                    _rtf.Append(string.Format("{0:X2}", _buffer[i]));
                }
                CS$1$0000 = _rtf.ToString();
            }
            finally
            {
                if (_graphics != null)
                {
                    _graphics.Dispose();
                }
                if (_metaFile != null)
                {
                    _metaFile.Dispose();
                }
                if (_stream != null)
                {
                    _stream.Close();
                }
            }
            return CS$1$0000;
        }

        public void InsertImage(Image _image)
        {
            StringBuilder _rtf = new StringBuilder();
            _rtf.Append(@"{\rtf1\ansi\ansicpg1252\deff0\deflang1033");
            _rtf.Append(this.GetFontTable(this.Font));
            _rtf.Append(this.GetImagePrefix(_image));
            _rtf.Append(this.GetRtfImage(_image));
            _rtf.Append(this.RTF_IMAGE_POST);
            base.SelectedRtf = _rtf.ToString();
        }

        public void InsertRtf(string _rtf)
        {
            base.SelectedRtf = _rtf;
        }

        public void InsertTextAsRtf(string _text)
        {
            this.InsertTextAsRtf(_text, this.Font);
        }

        public void InsertTextAsRtf(string _text, Font _font)
        {
            this.InsertTextAsRtf(_text, _font, this.textColor);
        }

        public void InsertTextAsRtf(string _text, Font _font, Color _textColor)
        {
            this.InsertTextAsRtf(_text, _font, _textColor, this.highlightColor);
        }

        public void InsertTextAsRtf(string _text, Font _font, Color _textColor, Color _backColor)
        {
            StringBuilder _rtf = new StringBuilder();
            _rtf.Append(@"{\rtf1\ansi\ansicpg1252\deff0\deflang1033");
            _rtf.Append(this.GetFontTable(_font));
            _rtf.Append(this.GetColorTable(_textColor, _backColor));
            _rtf.Append(this.GetDocumentArea(_text, _font));
            base.SelectedRtf = _rtf.ToString();
        }

        private string RemoveBadChars(string _originalRtf)
        {
            return _originalRtf.Replace("\0", "");
        }

        public Color HiglightColor
        {
            get
            {
                return this.highlightColor;
            }
            set
            {
                this.highlightColor = value;
            }
        }

        public string Rtf
        {
            get
            {
                return this.RemoveBadChars(base.Rtf);
            }
            set
            {
                base.Rtf = value;
            }
        }

        public Color TextColor
        {
            get
            {
                return this.textColor;
            }
            set
            {
                this.textColor = value;
            }
        }

        private enum EmfToWmfBitsFlags
        {
            EmfToWmfBitsFlagsDefault = 0,
            EmfToWmfBitsFlagsEmbedEmf = 1,
            EmfToWmfBitsFlagsIncludePlaceable = 2,
            EmfToWmfBitsFlagsNoXORClip = 4
        }

        [StructLayout(LayoutKind.Sequential, Size=1)]
        private struct RtfFontFamilyDef
        {
            public const string Unknown = @"\fnil";
            public const string Roman = @"\froman";
            public const string Swiss = @"\fswiss";
            public const string Modern = @"\fmodern";
            public const string Script = @"\fscript";
            public const string Decor = @"\fdecor";
            public const string Technical = @"\ftech";
            public const string BiDirect = @"\fbidi";
        }
    }
}

