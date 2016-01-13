using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace FaceRecControl
{
    /// <summary>
    /// MainWindow.xaml 的交互逻辑
    /// </summary>
    public partial class MainWindow : Window
    {
        [DllImport("FaceRec.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
        static extern int OpenPicture(StringBuilder fileName);
        [DllImport("FaceRec.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
        static extern int OpenDir(StringBuilder dirName);

        [DllImport("FaceRec.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
        static extern int GetGrayPicture(string srcName, StringBuilder destName);

        [DllImport("FaceRec.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
        static extern int GetThresholdPicture(string srcName, StringBuilder destName);
        public MainWindow()
        {
            FileName = "";
            InitializeComponent();
        }
        public ArrayList Flst
        {
            get;
            set;
        }
        string FileName
        {
            get;
            set;
        }
        ArrayList GetAll(DirectoryInfo dir)//搜索文件夹中的文件
        {
            ArrayList FileList = new ArrayList();

            FileInfo[] allFile = dir.GetFiles();
            foreach (FileInfo fi in allFile)
            {
                FileList.Add(fi.Name);
            }

            DirectoryInfo[] allDir = dir.GetDirectories();
            foreach (DirectoryInfo d in allDir)
            {
                GetAll(d);
            }
            return FileList;
        }
        public  void getDirFiles(string path)
        {
            DirectoryInfo d = new DirectoryInfo(path);
            Flst = GetAll(d);
        }


        private void OpenDir_Click(object sender, RoutedEventArgs e)
        {
            StringBuilder dirName = new StringBuilder(1024);
            try
            {
                if(OpenDir(dirName) == 0)
                {
                    getDirFiles(@"C:\Users\dell\Desktop\Pic");
                    for (int i = 0; i < Flst.Count; i++)
                    {
                        Console.Write(Flst[i]);
                    }
                }          
            }
            catch {
                MessageBox.Show("未知错误！");
            }
            
        }

        private void OpenFile_Click(object sender, RoutedEventArgs e)
        {
            StringBuilder fileName = new StringBuilder(1024);
            try
            {
                if (OpenPicture(fileName) == 0)
                {
                    Origin.Source = new BitmapImage(new Uri(fileName.ToString(), UriKind.Absolute));
                    FileName = fileName.ToString();
                }
            }
            catch
            {
                MessageBox.Show("请确认选择的文件是图片文件！");
            }
         }

        private void ProcessGray_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                StringBuilder fileName = new StringBuilder(1024);
                if(GetGrayPicture(FileName, fileName) == 0)
                    Gray_image.Source = new BitmapImage(new Uri(fileName.ToString(), UriKind.Absolute));
                else
                    MessageBox.Show("请确认已经选择文件！");       
            }
            catch {
                MessageBox.Show("请确认已经选择文件！");            
            }
         }

        private void ProcessThreshold_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                StringBuilder fileName = new StringBuilder(1024);
                if (GetThresholdPicture(FileName, fileName) == 0)
                    Threshold.Source = new BitmapImage(new Uri(fileName.ToString(), UriKind.Absolute));
                else
                    MessageBox.Show("请确认已经选择文件！");
            }
            catch
            {
                MessageBox.Show("请确认已经选择文件！");
            }
        }
    }
}
