using System;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using System.Windows.Forms;
using WMPLib;
using System.IO;

namespace MP3_Player
{
    public partial class MP3_Player : Form
    {
        // vector de perechi <nume melodie, locatie>
        private List<Tuple<string, string>> songs = new List<Tuple<string, string>>();

        // dictionat <playlist, vector de perechi <nume melodie, locatie>>
        private Dictionary<string, List<Tuple<string, string>>> playlists = new Dictionary<string, List<Tuple<string, string>>>();

        // un timer pt redarea melodiei urmatoare la sfarsitul melodiei redate actual
        private Timer timer = new Timer();

        // un timer pt actualizarea timpului melodiei afisat
        private Timer duration = new Timer();

        public MP3_Player()
        {
            InitializeComponent();

            SetTitle("Momentan nu se redă nicio melodie");

            // dezactivam butoanele de control
            EnableControls(false);

            // initializam playlist-urile folosind fisierul local "playlisturi.txt"
            InitializePlaylists();

            // initializam structura windowsMediaPlayer
            InitializePlayer();
        }

        private void EnableControls(bool enabled)
        {
            buttonPauza.Enabled = enabled;
            buttonRedare.Enabled = enabled;
            buttonOprire.Enabled = enabled;

            labelDuratie.Text = "";
        }

        private void InitializePlaylists()
        {
            // adaugam playlist-ul default
            listBoxPlaylisturi.Items.Add("Playlist spontan");
            listBoxPlaylisturi.SelectedIndex = 0;

            playlists.Add("Playlist spontan", songs);

            // verificam existenta fisierului local "playlisturi.txt"
            if (File.Exists("playlisturi.txt"))
            {
                try
                {
                    StreamReader reader = new StreamReader("playlisturi.txt");

                    // numarul de playlist-uri existente
                    int numPlaylists = int.Parse(reader.ReadLine());

                    for (int i = 0; i < numPlaylists; i++)
                    {
                        // despartim linia dupa virgula
                        string[] values = reader.ReadLine().Split(',');

                        // numele playlist-ului curent
                        string playlist = values[0];

                        // numarul de melodii din playlist
                        int numSongs = int.Parse(values[1]);

                        List<Tuple<string, string>> _songs = new List<Tuple<string, string>>();

                        for (int j = 0; j < numSongs * 2; j += 2)
                        {
                            // construieste tupla <nume melodie, locatie>
                            _songs.Add(new Tuple<string, string>(values[2 + j], values[3 + j]));
                        }

                        if (playlist == "Playlist spontan")
                        {
                            playlists[playlist] = _songs;
                            songs = _songs;
                                
                            // introdu numele melodiilor in lista
                            foreach (Tuple<string, string> song in _songs)
                            {
                                listBoxMelodii.Items.Add(song.Item1);
                                listBoxMelodii.SelectedIndex = listBoxMelodii.Items.Count - 1;
                            }

                            continue;
                        }

                        // asociaza lista de perechi <nume melodie, locatie> cu playlist-ul curent
                        playlists.Add(playlist, _songs);

                        // adauga numele playlist-ului la lista de playlist-uri
                        listBoxPlaylisturi.Items.Add(playlist);
                    }

                    reader.Close();
                }
                catch (Exception e)
                {
                    MessageBox.Show("Fișierul cu playlist-uri nu este formatat corect." + e.Message, "Eroare", MessageBoxButtons.OK);
                }
            }
        }

        private void InitializePlayer()
        {
            windowsMediaPlayer.settings.autoStart = false;

            // configureaza timer-ul pt redarea urmatoarei melodii
            timer.Interval = 10;
            timer.Stop();
            timer.Tick += new EventHandler(timer_Tick);

            // configureaza timer-ul pt afisarea timpului melodiei
            duration.Interval = 500;
            duration.Stop();
            duration.Tick += new EventHandler(duration_Tick);

            // reseteaza text-ul pt timpul melodiei
            labelDuratie.Text = "";
        }

        private void Save()
        {
            try
            {
                StreamWriter writer = new StreamWriter("playlisturi.txt");
                // scrie in fisier numarul de playlist-uri
                writer.WriteLine(playlists.Count);

                for (int i = 0; i < playlists.Count; i++)
                {
                    string line = "";
                    string playlist = listBoxPlaylisturi.Items[i].ToString();

                    // numarul de melodii din playlist
                    int numSongs = playlists[playlist].Count;

                    // "<nume playlist>,<nr melodii>,"
                    line += playlist + "," + numSongs + ",";

                    for (int j = 0; j < playlists[playlist].Count; j++)
                    {
                        string song = playlists[playlist][j].Item1;
                        string location = playlists[playlist][j].Item2;

                        // "<nume melodie>,<locatie>,"
                        line += song + "," + location + ",";
                    }

                    writer.WriteLine(line);
                }

                writer.Close();

                MessageBox.Show("Salvarea playlist-urilor a reușit.", "Succes", MessageBoxButtons.OK);
            }
            catch (Exception)
            {
                MessageBox.Show("Salvarea playlist-urilor a eșuat.", "Eroare", MessageBoxButtons.OK);
            }
            
        }

        private string GetLocation(string name)
        {
            // cauta in lista de perechi <nume melodie, locatie> locatia asosciata unei melodii
            for (int i = 0; i < songs.Count; i++)
            {
                // am gasit melodia
                if (songs[i].Item1 == name)
                {
                    // returneaza locatia
                    return songs[i].Item2;
                }
            }

            return string.Empty;
        }

        private void AddSong(string name, string fullName)
        {
            try
            {
                // construim perechea <nume melodie, locatie>
                Tuple<string, string> song = new Tuple<string, string>(name, fullName);
                // o adaugam in lista de perechi
                songs.Add(song);

                // adaugam numele melodiei in lista
                listBoxMelodii.Items.Add(name);
                // reactualizam selectia
                listBoxMelodii.SelectedIndex = listBoxMelodii.Items.Count - 1;
            }
            catch (Exception exception)
            {
                MessageBox.Show(exception.Message, "Eroare", MessageBoxButtons.OK);
            }
        }

        private void RemoveSong()
        {
            if (listBoxMelodii.Items.Count > 0)
            {
                string song = listBoxMelodii.Items[listBoxMelodii.SelectedIndex].ToString();
                string location = GetLocation(song);
                string playlist = listBoxPlaylisturi.Items[listBoxPlaylisturi.SelectedIndex].ToString();

                // numele melodiei + [playlist-ul din care provine]
                // daca sirul obtinut mai sus este identic cu sirul afisat pe ecran (melodia care se reda actual)
                // atunci oprim melodia inainte sa o stergem
                if (song + " [" + playlist + "]" == labelMelodie.Text)
                {
                    windowsMediaPlayer.Ctlcontrols.stop();

                    buttonPauza.Enabled = false;
                    buttonPauza.Text = "Pauză";

                    buttonOprire.Enabled = false;

                    SetTitle("Momentan nu se redă nicio melodie");

                    labelDuratie.Text = "";
                    duration.Stop();
                }

                // stergem melodia din lista de perechi <nume melodie, locatie>
                songs.RemoveAt(listBoxMelodii.SelectedIndex);

                // stergem melodia din lista grafica de melodii
                int index = listBoxMelodii.SelectedIndex;
                listBoxMelodii.Items.RemoveAt(index);

                // reactualizam selectia din lista
                int count = listBoxMelodii.Items.Count;
                if (count > 0) listBoxMelodii.SelectedIndex = (index % count + count - 1) % count;
            }
        }

        private void PlaySong()
        {
            try
            {
                string song = listBoxMelodii.Items[listBoxMelodii.SelectedIndex].ToString();
                string location = GetLocation(song);

                // pornim melodia
                windowsMediaPlayer.Ctlcontrols.stop();
                windowsMediaPlayer.URL = location;
                windowsMediaPlayer.Ctlcontrols.play();

                string playlist = listBoxPlaylisturi.Items[listBoxPlaylisturi.SelectedIndex].ToString();
                // afiseaza numele melodiei si numele playlist-ului din care provine
                SetTitle(song + " [" + playlist + "]");

                // activeaza butoanele
                EnableControls(true);

                buttonPauza.Text = "Pauză";

                duration.Start();
            }
            catch (Exception)
            {
                MessageBox.Show("Fișierul nu este formatat potrivit sau este corupt.", "Eroare", MessageBoxButtons.OK);
            }
        }

        private void SetTitle(string title)
        {
            labelMelodie.Text = title;
        }

        private void NextSong(bool forward = true)
        {
            int count = listBoxMelodii.Items.Count;

            if (count > 1)
            {
                int index = listBoxMelodii.FindString(labelMelodie.Text.Split('[')[0].Trim());

                // mutam catre melodia din fata
                if (forward)
                {
                    listBoxMelodii.SelectedIndex = (index + 1) % count;
                }
                // mutam catre melodia din spate
                else
                {
                    listBoxMelodii.SelectedIndex = (index % count + count - 1) % count;
                }

                PlaySong();
            }
        }

        private void buttonAdaugaMelodie_Click(object sender, EventArgs e)
        {
            // deschidem file explorer-ul configuram ca sa accepte doar .mp3
            OpenFileDialog openFileDialog = new OpenFileDialog();
            openFileDialog.CheckFileExists = true;
            openFileDialog.Title = "Adaugă o melodie";
            openFileDialog.Filter = "MP3 files (*.mp3)|*.mp3";
            openFileDialog.DefaultExt = ".mp3";
            openFileDialog.Multiselect = true;

            DialogResult result = openFileDialog.ShowDialog();

            if (result == DialogResult.OK)
            {
                foreach (string fileName in openFileDialog.FileNames)
                {
                    // preluam numele si locatia fisierului
                    System.IO.FileInfo fileInfo = new System.IO.FileInfo(fileName);

                    AddSong(fileInfo.Name, fileInfo.FullName);
                }
            }
            else if (result != DialogResult.Cancel)
            {
                MessageBox.Show("Eroare la adăugarea melodiei.", "Eroare", MessageBoxButtons.OK);
            }
        }

        private void buttonRedare_Click(object sender, EventArgs e)
        {
            PlaySong();
        }

        private void buttonPauza_Click(object sender, EventArgs e)
        {
            if (buttonPauza.Text == "Pauză")
            {
                windowsMediaPlayer.Ctlcontrols.pause();
                buttonPauza.Text = "Reia";

                duration.Stop();
            }
            else
            {
                windowsMediaPlayer.Ctlcontrols.play();
                buttonPauza.Text = "Pauză";

                duration.Start();
            }
        }

        private void listBoxMelodii_SelectedIndexChanged(object sender, EventArgs e)
        {
            // exista melodii in playlist -> activam butonul de redare
            if (listBoxMelodii.Items.Count > 0)
            {
                buttonRedare.Enabled = true;
            }
            // altfel il dezactivam
            else
            {
                EnableControls(false);
            }
        }

        private void buttonStergeMelodie_Click(object sender, EventArgs e)
        {
            RemoveSong();
        }

        private void MP3_Player_DragDrop(object sender, DragEventArgs e)
        {
            // preia numele fisierelor trase in aplicatie
            string[] fileNames = (string[])e.Data.GetData(DataFormats.FileDrop, false);

            // itereaza prin fiecare, verifica daca e .mp3 si preia numele fisierului
            foreach (string fileName in fileNames)
            {
                if (fileName.ToLower().EndsWith(".mp3"))
                {
                    string name = "";
                    for (int i = 0; i < fileName.Length; i++)
                    {
                        if (fileName[i] == '\\' || fileName[i] == '/')
                        {
                            name = "";
                        }
                        else
                        {
                            name += fileName[i];
                        }
                    }

                    AddSong(name, fileName);
                }
            }

            // schimba imaginea
            pictureBoxMelodie.Image = Properties.Resources.musical_note;
        }

        private void MP3_Player_DragEnter(object sender, DragEventArgs e)
        {
            // schimba imaginea pt a indica actiunea dorita
            pictureBoxMelodie.Image = Properties.Resources.playlist;

            if (e.Data.GetDataPresent(DataFormats.FileDrop))
            {
                e.Effect = DragDropEffects.Copy;
            }
            else
            {
                e.Effect = DragDropEffects.None;
            }
        }

        private void MP3_Player_DragLeave(object sender, EventArgs e)
        {
            // revino la imaginea initiala
            pictureBoxMelodie.Image = Properties.Resources.musical_note;
        }

        private void buttonInainte_Click(object sender, EventArgs e)
        {
            NextSong();
        }

        private void buttonInapoi_Click(object sender, EventArgs e)
        {
            NextSong(false);
        }

        private void buttonOprire_Click(object sender, EventArgs e)
        {
            windowsMediaPlayer.Ctlcontrols.stop();

            buttonPauza.Enabled = false;
            buttonPauza.Text = "Pauză";

            buttonOprire.Enabled = false;

            SetTitle("Momentan nu se redă nicio melodie");

            labelDuratie.Text = "";
            duration.Stop();
        }

        private void timer_Tick(object sender, EventArgs e)
        {
            timer.Stop();

            duration.Stop();

            NextSong();
        }

        private void windowsMediaPlayer_PlayStateChange(object sender, AxWMPLib._WMPOCXEvents_PlayStateChangeEvent e)
        {
            if (windowsMediaPlayer.playState == WMPPlayState.wmppsMediaEnded)
            {
                timer.Start();
            }
        }

        private void textBoxCautare_Enter(object sender, EventArgs e)
        {
            if (textBoxCautare.Text == "Căutare...")
            {
                textBoxCautare.ForeColor = Color.Black;
                textBoxCautare.Text = "";
            }
        }

        private void textBoxCautare_Leave(object sender, EventArgs e)
        {
            if (textBoxCautare.Text == "")
            {
                textBoxCautare.ForeColor = Color.DimGray;
                textBoxCautare.Text = "Căutare...";
            }
        }

        private void textBoxCautare_TextChanged(object sender, EventArgs e)
        {
            // se efectueaza cautarea ori de cate ori se schimba input-ul
            if (textBoxCautare.Text.Length > 0)
            {
                // itereaza prin lista de melodii
                for (int i = 0; i < listBoxMelodii.Items.Count; i++)
                {
                    string name = listBoxMelodii.Items[i].ToString();

                    // numele melodiei contine ca substring input-ul dat
                    if (name.ToLower().Contains(textBoxCautare.Text.ToLower()))
                    {
                        // selecteaza melodia
                        listBoxMelodii.SetSelected(i, true);
                    }
                }
            }
        }

        private void duration_Tick(object sender, EventArgs e)
        {
            // actualizeaza timpul melodiei
            try
            {
                if (windowsMediaPlayer.playState == WMPPlayState.wmppsPlaying)
                {    
                    labelDuratie.Text = windowsMediaPlayer.Ctlcontrols.currentPositionString + "/" + windowsMediaPlayer.currentMedia.durationString;
                }
            }
            catch(Exception)
            {
                return;
            }
        }

        private void buttonCreeaza_Click(object sender, EventArgs e)
        {
            string name = textBoxPlaylist.Text;

            // numele nu trebuie sa se repete iar lungimea > 0
            if (name.Length > 0 && listBoxPlaylisturi.FindStringExact(name) == -1)
            {
                listBoxPlaylisturi.Items.Add(name);

                textBoxPlaylist.Text = "";

                // numele playlist-ului selectat actual
                string actual = listBoxPlaylisturi.Items[listBoxPlaylisturi.SelectedIndex].ToString();

                // salveaza lista de melodii asociata playlist-ului selctat actual
                playlists[actual] = songs;
                playlists.Add(name, new List<Tuple<string, string>>());

                // adauga un nou playlist in lista de playlist-uri
                listBoxPlaylisturi.SelectedIndex = listBoxPlaylisturi.Items.Count - 1;
            }
        }

        private void buttonSterge_Click(object sender, EventArgs e)
        {
            string playlist = listBoxPlaylisturi.Items[listBoxPlaylisturi.SelectedIndex].ToString();

            // playlist-ul default nu poate fi sters
            if (playlist != "Playlist spontan")
            {
                // stergem din lista de playlist-uri
                playlists.Remove(playlist);

                // stergem din lista grafica
                int index = listBoxPlaylisturi.SelectedIndex;
                listBoxPlaylisturi.Items.RemoveAt(index);

                // actualizam selectia pe lista
                int count = listBoxPlaylisturi.Items.Count;
                if (count > 0) listBoxPlaylisturi.SelectedIndex = (index % count + count - 1) % count;
            }
        }

        private void listBoxPlaylisturi_SelectedValueChanged(object sender, EventArgs e)
        {
            // actualizeaza lista de melodii asociate playlist-ului curent
            listBoxMelodii.Items.Clear();

            if (listBoxPlaylisturi.SelectedIndex < 0) listBoxPlaylisturi.SelectedIndex = 0;

            string name = listBoxPlaylisturi.Items[listBoxPlaylisturi.SelectedIndex].ToString();

            if (!playlists.ContainsKey(name)) return;

            // reatribuie melodiile corespondente playlist-ului selectat
            songs = playlists[name];

            // adauga-le in lista grafica
            foreach (Tuple<string, string> song in songs)
            {
                listBoxMelodii.Items.Add(song.Item1);
                listBoxMelodii.SelectedIndex = listBoxMelodii.Items.Count - 1;
            }
        }

        private void menuItemSalveaza_Click(object sender, EventArgs e)
        {
            //string[] names = listBoxPlaylisturi.Items.OfType<string>().ToArray();
            //Salvare.Salvare.Save(names, playlists);
            Save();
        }

        private void MP3_Player_FormClosing(object sender, FormClosingEventArgs e)
        {
            //string[] names = listBoxPlaylisturi.Items.OfType<string>().ToArray();
            //Salvare.Salvare.Save(names, playlists);
            Save();
        }

        private void menuItemAjutor_Click(object sender, EventArgs e)
        {
            try
            {
                System.Diagnostics.Process.Start("MP3 Player Docs.chm");
            }
            catch (Exception)
            {
                MessageBox.Show("Nu a putut fi gasit fisierul de ajutor.", "Eroare", MessageBoxButtons.OK);
            }
        }
    }
}
