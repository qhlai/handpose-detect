namespace NetDebugTool
{
    using System;
    using System.Drawing;
    using System.Threading;
    using System.Windows.Forms;

    public class SplashForm : Form
    {
        private static Bitmap m_bmp;
        private DelegateCloseSplash m_delegateClose;
        public static SplashForm m_instance;

        public SplashForm()
        {
            base.FormBorderStyle = FormBorderStyle.None;
            base.ShowInTaskbar = false;
            base.TransparencyKey = this.BackColor;
            base.Width = m_bmp.Width;
            base.Height = m_bmp.Height;
            base.StartPosition = FormStartPosition.CenterScreen;
            base.Paint += new PaintEventHandler(this.SplashForm_Paint);
            this.m_delegateClose = new DelegateCloseSplash(this.InternalCloseSplash);
            this.BackgroundImage = m_bmp;
        }

        public static void CloseSplash()
        {
            if (m_instance != null)
            {
                m_instance.Invoke(m_instance.m_delegateClose);
            }
        }

        protected override void Dispose(bool disposing)
        {
            m_bmp.Dispose();
            base.Dispose(disposing);
            m_instance = null;
        }

        private void InitializeComponent()
        {
            base.SuspendLayout();
            base.Name = "SplashForm";
            base.ResumeLayout(false);
        }

        private void InternalCloseSplash()
        {
            base.Close();
            base.Dispose();
        }

        private static void MySplashThreadFunc()
        {
            m_instance = new SplashForm();
            m_instance.ShowDialog();
        }

        private void SplashForm_Paint(object sender, PaintEventArgs e)
        {
            e.Graphics.DrawString("载入中...", new Font("宋体", 13f, FontStyle.Bold), new SolidBrush(Color.FromArgb(12, 100, 12)), new PointF(16f, 39f));
        }

        public static void StartSplash(Bitmap bmp)
        {
            m_bmp = bmp;
            new Thread(new ThreadStart(SplashForm.MySplashThreadFunc)).Start();
        }
    }
}

