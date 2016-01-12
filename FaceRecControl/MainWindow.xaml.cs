using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Linq;
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
        Vector s;
        public MainWindow()
        {
          
            InitializeComponent();
        }
        public ArrayList Flst
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
            getDirFiles(@"C:\Users\dell\Desktop\Pic");
            for (int i = 0; i < Flst.Count; i++ )
            {
                Console.Write(Flst[i]);
            }
        }
    }
}
