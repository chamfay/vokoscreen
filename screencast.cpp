/* vokoscreen - A desktop recorder
 * Copyright (C) 2011-2013 Volker Kohaupt
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
#include "screencast.h"  

using namespace std;

screencast::screencast()
{
    bool beta = true;
    QString Beta;
    if ( beta )
      Beta = "Beta";
    else
      Beta = "";

    ProgName = "vokoscreen";
    Version = "1.7.22";  
    Version = Version + " " + Beta;

    homepage = "<a href='http://www.kohaupt-online.de/hp'>" + tr( "Homepage" ) + "</a>";
    
    QString mailingliste = "<a href ='mailto:vokoscreen@googlegroups.com?subject=vokoscreen ";
    mailingliste = mailingliste.append( Version ).append( "'" ).append( ">" + tr( "Mailinglist" ) + "</a>" );
    
    email = "<a href ='mailto:tux@kohaupt-online.de?subject=vokoscreen ";
    email = email.append( Version ).append( "'" ).append( ">" + tr( "Support" ) + "</a>" );

    QString emaildeveloper = "<a href ='mailto:vkohaupt@freenet.de?subject=vokoscreen ";
    emaildeveloper = emaildeveloper.append( Version ).append( "'" ).append( ">" + tr( "Developer" ) + "</a>" );
    
    screencast::setWindowTitle( ProgName + " " + Version );

    QIcon icon;
    icon.addFile( QString::fromUtf8( ":/pictures/vokoscreen.png" ), QSize(), QIcon::Normal, QIcon::Off );
    screencast::setWindowIcon( icon );

    qDebug() << "[vokoscreen]" << "Version:" << Version;
    qDebug() << "[vokoscreen]" << "Qt Version: " << qVersion();
    QvkAlsaDevice inBox ;
    qDebug() << "[vokoscreen]" << "asoundlib Version:" << inBox.getAlsaVersion();
    qDebug() << "[vokoscreen]" << "ffmpeg Version:" << getFfmpegVersion();
    qDebug();
    
    searchExternalPrograms();
    
    pause = false;
    firststartWininfo = false;
    
    QWidget *centralWidget = new QWidget( this );
    centralWidget->setObjectName( QString::fromUtf8( "centralWidget" ) );
    this->setCentralWidget( centralWidget );

    QHBoxLayout *hboxGlobal = new QHBoxLayout(centralWidget);

    QLabel* label = new QLabel;
    label->setPixmap(QPixmap(":/pictures/VokoCola.png"));
    label->setScaledContents( true );
    hboxGlobal->addWidget(label, 0, Qt::AlignCenter);

    QVBoxLayout *vboxGlobal = new QVBoxLayout;
    hboxGlobal->addLayout(vboxGlobal);

    tabWidget = new QTabWidget;
    tabWidget->setIconSize( QSize( 40, 40 ) );
    vboxGlobal->addWidget(tabWidget);

    // Tab 1 Screen options ***************************************************
    QFrame *frame = new QFrame( this );
    tabWidget->addTab( frame, "" );
    tabWidget->setTabIcon( 0, QIcon( ":/pictures/monitor.png" ) );
    QFont qfont = frame->font();
    qfont.setPixelSize( 12 );
    frame->setFont( qfont );

    QHBoxLayout *hboxTab1 = new QHBoxLayout(frame);
    hboxTab1->setMargin(20);
    QVBoxLayout *vboxWindow = new QVBoxLayout;
    hboxTab1->addLayout(vboxWindow);
 
    FullScreenRadioButton = new QRadioButton;
    FullScreenRadioButton->setText( tr( "Fullscreen" ) );
    FullScreenRadioButton->setChecked( true );
    connect( FullScreenRadioButton, SIGNAL( clicked() ), SLOT( clickedScreenSize() ) );
    vboxWindow->addWidget(FullScreenRadioButton);

    WindowRadioButton = new QRadioButton;
    WindowRadioButton->setText( tr( "Window" ) );
    connect( WindowRadioButton, SIGNAL( clicked() ), SLOT( clickedScreenSize() ) );
    vboxWindow->addWidget(WindowRadioButton);

    AreaRadioButton = new QRadioButton;
    AreaRadioButton->setText( tr( "Area" ) );
    connect( AreaRadioButton, SIGNAL( clicked() ), SLOT( clickedScreenSize() ) );
    vboxWindow->addWidget(AreaRadioButton);

    vboxWindow->addSpacing(20);

    QFrame *vline = new QFrame(frame);
    vline->setFrameShape(QFrame::VLine);
    hboxTab1->addWidget(vline);

    hboxTab1->addSpacing(40);

    QVBoxLayout *vboxOptions = new QVBoxLayout;
    hboxTab1->addLayout(vboxOptions);
    
    QHBoxLayout *hboxMagn = new QHBoxLayout;
    vboxOptions->addLayout(hboxMagn);

    MagnifierCheckBox = new QCheckBox;
    MagnifierCheckBox->setText( tr( "Magnification" ) );
    MagnifierCheckBox->setToolTip( "CTRL+SHIFT+F9" );
    hboxMagn->addWidget(MagnifierCheckBox, 0, Qt::AlignLeft);

    QPushButton *MagnifierDialogPushButton = new QPushButton("...");
    connect( MagnifierDialogPushButton, SIGNAL( clicked() ), SLOT( MagnifierDialog() ) );
    hboxMagn->addWidget(MagnifierDialogPushButton, 0, Qt::AlignLeft);

    webcamCheckBox = new QvkWebcamController;
    vboxOptions->addLayout(webcamCheckBox);

    vboxOptions->addSpacing(20);

    QHBoxLayout *hboxCount = new QHBoxLayout;
    vboxOptions->addLayout(hboxCount);

    QLabel *CountdownLabel = new QLabel;
    CountdownLabel->setText( tr( "Countdown" ) );
    hboxCount->addWidget(CountdownLabel);
    
    CountdownSpinBox = new QSpinBox;
    CountdownSpinBox->setMinimum( 0 );
    CountdownSpinBox->setMaximum( 999 );
    CountdownSpinBox->setSingleStep( 1 );
    CountdownSpinBox->setValue( 0 );
    hboxCount->addWidget(CountdownSpinBox,0 , Qt::AlignLeft);
    

    // Tab 2 Audio options ****************************************
    TabWidgetAudioFrame = new QFrame( this );
    tabWidget->addTab( TabWidgetAudioFrame, "" );
    tabWidget->setTabIcon( 1, QIcon( ":/pictures/micro.png" ) );
    qfont = TabWidgetAudioFrame->font();
    qfont.setPixelSize( 12 );
    TabWidgetAudioFrame->setFont( qfont );

    QVBoxLayout *vboxAudio = new QVBoxLayout(TabWidgetAudioFrame);
    vboxAudio->setMargin(8);

    AudioOnOffCheckbox = new QCheckBox( tr( "Audio" ) );
    vboxAudio->addWidget(AudioOnOffCheckbox);
    connect( AudioOnOffCheckbox,  SIGNAL( stateChanged( int ) ), SLOT( stateChangedAudio( int ) ) );

    gridAudio = new QGridLayout;
    vboxAudio->addLayout(gridAudio);
    gridAudio->setContentsMargins(10, 0, 0, 0);

    PulseDeviceRadioButton = new QRadioButton( tr( "Pulse" ) );
    gridAudio->addWidget(PulseDeviceRadioButton, 0, 0, Qt::AlignTop);
    connect( PulseDeviceRadioButton,  SIGNAL( clicked( bool )  ), SLOT( clickedAudioPulse( bool ) ) );

    Pulseframe = new QFrame;

    QHBoxLayout *hboxAlsa = new QHBoxLayout;
    vboxAudio->addLayout(hboxAlsa);

    AlsaRadioButton = new QRadioButton( tr( "Alsa" ) );
    gridAudio->addWidget(AlsaRadioButton, 1, 0);
    connect( AlsaRadioButton,  SIGNAL( clicked( bool )  ), SLOT( clickedAudioAlsa( bool ) ) );
    
    AlsaHwComboBox = new QComboBox( TabWidgetAudioFrame );
    gridAudio->addWidget(AlsaHwComboBox, 1, 1);

    // Tab 3 Video options **************************************************
    TabWidgetVideoOptionFrame = new QFrame( this );
    tabWidget->addTab( TabWidgetVideoOptionFrame, "" );
    tabWidget->setTabIcon( 2, QIcon( ":/pictures/videooptionen.png" ) );
    qfont = TabWidgetVideoOptionFrame->font();
    qfont.setPixelSize( 12 );
    TabWidgetVideoOptionFrame->setFont( qfont );

    QVBoxLayout *vboxTab3 = new QVBoxLayout(TabWidgetVideoOptionFrame);
    vboxTab3->setMargin(14);

    QHBoxLayout *hboxFrames = new QHBoxLayout;
    vboxTab3->addLayout(hboxFrames);

    QLabel *VideoOptionLabel = new QLabel(tr( "Frames" ));
    hboxFrames->addWidget(VideoOptionLabel);

    FrameSpinBox = new QSpinBox;
    FrameSpinBox->setMinimum( 1 );
    FrameSpinBox->setMaximum( 99 );
    FrameSpinBox->setSingleStep( 1 );
    FrameSpinBox->setValue( 25 );
    connect( FrameSpinBox, SIGNAL( valueChanged( int ) ), SLOT( valueChangedFrames( int ) ) );
    hboxFrames->addWidget(FrameSpinBox);

    hboxFrames->addStretch();

    QPushButton *FrameStandardButton = new QPushButton( tr( "Default" ) );
    connect( FrameStandardButton, SIGNAL( clicked() ), SLOT( setFrameStandardSpinbox() ) );
    hboxFrames->addWidget(FrameStandardButton);

    QHBoxLayout *hboxVideoOpts = new QHBoxLayout;
    vboxTab3->addLayout(hboxVideoOpts);

    QLabel *VideocodecOptionLabel = new QLabel( tr( "Codec" ) );
    VideocodecOptionLabel->setMinimumWidth(42);
    hboxVideoOpts->addWidget(VideocodecOptionLabel);

    VideocodecComboBox = new QComboBox;
    VideocodecComboBox->addItem( "mpeg4" );
    VideocodecComboBox->addItem( "libx264" );
    connect( VideocodecComboBox, SIGNAL( currentIndexChanged( int ) ), SLOT( currentIndexChangedCodec( int ) ) );
    hboxVideoOpts->addWidget(VideocodecComboBox);

    hboxVideoOpts->addSpacing(20);
    
    QLabel *VideoContainerLabel = new QLabel( tr( "Format" ) );
    hboxVideoOpts->addWidget(VideoContainerLabel);

    hboxVideoOpts->addSpacing(4);
    
    VideoContainerComboBox = new QComboBox;
    VideoContainerComboBox->addItem( "avi" );
    VideoContainerComboBox->addItem( "mkv" );
    connect( VideoContainerComboBox, SIGNAL( currentIndexChanged( int ) ), SLOT( currentIndexChangedFormat( int ) ) );
    hboxVideoOpts->addWidget(VideoContainerComboBox);
    
    hboxVideoOpts->addStretch();

    QPushButton *VideocodecStandardButton = new QPushButton( tr( "Default" ) );
    connect( VideocodecStandardButton, SIGNAL( clicked() ), SLOT( setVideocodecStandardComboBox() ) );
    hboxVideoOpts->addWidget(VideocodecStandardButton);
    
    vboxTab3->addSpacing(20);

    HideMouseCheckbox = new QCheckBox( tr( "Do not record mouse cursor" ) );
    vboxTab3->addWidget(HideMouseCheckbox);

    // Tab 4 Miscellaneous options **************************************************
    TabWidgetMiscellaneousFrame = new QFrame( this );
    tabWidget->addTab(TabWidgetMiscellaneousFrame, "" );
    tabWidget->setTabIcon( 3, QIcon( ":/pictures/tools.png" ) );
    qfont = TabWidgetMiscellaneousFrame->font();
    qfont.setPixelSize( 12 );
    TabWidgetMiscellaneousFrame->setFont( qfont );

    QVBoxLayout *vboxTab4 = new QVBoxLayout(TabWidgetMiscellaneousFrame);
    vboxTab4->setMargin(14);

    QHBoxLayout *hboxPath = new QHBoxLayout;
    vboxTab4->addLayout(hboxPath);

    QLabel *SaveVideoPathLabel = new QLabel( tr( "Videopath" ) );
    SaveVideoPathLabel->setMinimumWidth(65);
    hboxPath->addWidget(SaveVideoPathLabel);

    SaveVideoPathLineEdit = new QLineEdit;
    SaveVideoPathLineEdit->setReadOnly( true );
    hboxPath->addWidget(SaveVideoPathLineEdit);
    
    QPushButton *SaveVideoPathPushButton = new QPushButton( "..." );
    connect( SaveVideoPathPushButton, SIGNAL(clicked() ), SLOT( saveVideoPath() ) );
    hboxPath->addWidget(SaveVideoPathPushButton);

    QHBoxLayout *hboxPlayer = new QHBoxLayout;
    vboxTab4->addLayout(hboxPlayer);
    
    QLabel *VideoPlayerLabel = new QLabel( tr( "Player" ) );
    VideoPlayerLabel->setMinimumWidth(65);
    hboxPlayer->addWidget(VideoPlayerLabel);
    
    VideoplayerComboBox = new QComboBox;
    hboxPlayer->addWidget(VideoplayerComboBox, 0, Qt::AlignLeft);
    hboxPlayer->addStretch();

    MinimizedCheckBox = new QCheckBox( tr( "Vokoscreen minimized when recording starts" ) );
    vboxTab4->addWidget(MinimizedCheckBox);

    // Tab 6 About *********************************************************
    QFrame *TabWidgetAboutFrame = new QFrame(this);
    tabWidget->addTab( TabWidgetAboutFrame, "" );
    tabWidget->setTabIcon( 4, QIcon( ":/pictures/about.png" ) );
    qfont = TabWidgetAboutFrame->font();
    qfont.setPixelSize( 12 );
    TabWidgetAboutFrame->setFont( qfont );

    QGridLayout *gridAbout = new QGridLayout(TabWidgetAboutFrame);
    gridAbout->setMargin(14);
    
    QLabel* labelWebSite = new QLabel( TabWidgetAboutFrame );
    labelWebSite->setText( homepage );
    labelWebSite->setOpenExternalLinks( true );
    gridAbout->addWidget(labelWebSite, 0, 0, Qt::AlignCenter);

    QLabel* labelMailinglisteMail = new QLabel( TabWidgetAboutFrame );
    labelMailinglisteMail->setText( mailingliste );
    labelMailinglisteMail->setOpenExternalLinks( true );
    gridAbout->addWidget(labelMailinglisteMail, 1, 0, Qt::AlignCenter);

    QLabel* labelMail = new QLabel( TabWidgetAboutFrame );
    labelMail->setText( email );
    labelMail->setOpenExternalLinks( true );
    gridAbout->addWidget(labelMail, 2, 0, Qt::AlignCenter);

    QLabel* labelDeveLoperMail = new QLabel( TabWidgetAboutFrame );
    labelDeveLoperMail->setText( emaildeveloper );
    labelDeveLoperMail->setOpenExternalLinks( true );
    gridAbout->addWidget(labelDeveLoperMail, 3, 0, Qt::AlignCenter);
    
    QLabel* labelLanguageUrl = new QLabel( TabWidgetAboutFrame );
    labelLanguageUrl->setText( "<a href='https://www.transifex.com/projects/p/vokoscreen/'>" + tr( "Translations" ) + "</a>" );
    labelLanguageUrl->setOpenExternalLinks( true );
    gridAbout->addWidget(labelLanguageUrl, 0, 2, Qt::AlignCenter);
    
    QLabel* labelOpensuseBetaUrl = new QLabel( TabWidgetAboutFrame );
    labelOpensuseBetaUrl->setText( "<a href='http://linuxecke.volkoh.de/vokoscreen/vokoscreen.html'>" + tr( "Beta openSUSE" ) + "</a>" );
    labelOpensuseBetaUrl->setOpenExternalLinks( true );
    gridAbout->addWidget(labelOpensuseBetaUrl, 1, 2, Qt::AlignCenter);

    QLabel* labelUbuntuBetaUrl = new QLabel( TabWidgetAboutFrame );
    labelUbuntuBetaUrl->setText( "<a href='http://ppa.launchpad.net/vokoscreen-dev/vokoscreen-daily/ubuntu/pool/main/v/vokoscreen/'>" + tr( "Beta Ubuntu" ) + "</a>" );
    labelUbuntuBetaUrl->setOpenExternalLinks( true );
    gridAbout->addWidget(labelUbuntuBetaUrl, 2, 2, Qt::AlignCenter);
    
    QLabel * labelDonateUrl = new QLabel( TabWidgetAboutFrame );
    labelDonateUrl->setText( "<a href='http://www.kohaupt-online.de/hp/spende.html'>" + tr( "Donate" ) + "</a>" );
    labelDonateUrl->setOpenExternalLinks( true );
    gridAbout->addWidget(labelDonateUrl, 4, 1, Qt::AlignCenter);
    //labelDonateUrl->setAutoFillBackground( true ); //******************
    //labelDonateUrl->setPalette( palette );         //******************

    // End Tabs *************************************************************

    QHBoxLayout * hboxControls = new QHBoxLayout;
    vboxGlobal->addLayout(hboxControls);

    hboxControls->addStretch();

    int btnsHeight = 30;

    recordButton = new QPushButton( tr( "Start" ) );
    recordButton->setToolTip( "CTRL+SHIFT+F10" );
    recordButton->setMinimumHeight(btnsHeight);
    qfont = recordButton->font();
    qfont.setPixelSize( 14 );
    qfont.setBold( true );
    recordButton->setFont( qfont );
    hboxControls->addWidget(recordButton);

/*    if ( needProgram( "ffmpeg" ) )  *************************************************************************************
      recordButton->setEnabled( true );
    else
      recordButton->setEnabled( false );
*/
    connect( recordButton, SIGNAL( clicked() ), SLOT( preRecord() ) );

    StopButton = new QPushButton( tr( "Stop" ) );
    StopButton->setMinimumHeight(btnsHeight);
    StopButton->setToolTip( "CTRL+SHIFT+F11" );
    hboxControls->addWidget(StopButton);
    qfont = StopButton->font();
    qfont.setPixelSize( 14 );
    qfont.setBold( true );
    StopButton->setFont( qfont );
    StopButton->setEnabled( false );
    connect( StopButton, SIGNAL( clicked() ), SLOT( Stop() ) );
    
    PauseButton = new QPushButton( tr( "Pause" ) );
    PauseButton->setMinimumHeight(btnsHeight);
    PauseButton->setToolTip( "CTRL+SHIFT+F12" );
    hboxControls->addWidget(PauseButton);
    qfont = PauseButton->font();
    qfont.setPixelSize( 14 );
    qfont.setBold( true );
    PauseButton->setFont( qfont );
    PauseButton->setCheckable( true );
    PauseButton->setEnabled( false );

    if ( needProgram( "mkvmerge" ) )
      PauseButton->show();
    else
      PauseButton->hide();
    connect( PauseButton, SIGNAL( clicked() ), SLOT( Pause() ) );

    PlayButton = new QPushButton( tr( "Play" ) );
    PlayButton->setMinimumHeight(btnsHeight);
    PlayButton->setToolTip( tr( "Play last Video" ) );
    hboxControls->addWidget(PlayButton);
    qfont = PlayButton->font();
    qfont.setPixelSize( 14 );
    qfont.setBold( true );
    PlayButton->setFont( qfont );
    connect( PlayButton, SIGNAL( clicked() ), SLOT( play() ) );

    sendPushButton = new QPushButton( tr( "Send" ) );
    sendPushButton->setMinimumHeight(btnsHeight);
    sendPushButton->setToolTip( tr( "Send Video" ) );
    hboxControls->addWidget(sendPushButton);
    qfont = sendPushButton->font();
    qfont.setPixelSize( 14 );
    qfont.setBold( true );
    sendPushButton->setFont( qfont );
    connect( sendPushButton, SIGNAL( clicked() ), SLOT( send() ) );
    if ( needProgram( "xdg-email" ) )
      sendPushButton->setEnabled( true );
    else
      sendPushButton->setEnabled( false );

    hboxControls->addStretch();

    // Statusbar
    statusBarProgForRecord = new QLabel();
    statusBarProgForRecord->setText( recordApplikation );
    statusBarProgForRecord->setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );
    statusBarProgForRecord->setToolTip( tr ( "Program for recording" ) );
    
    statusBarLabelTime = new QLabel();
    statusBarLabelTime->setText( "00:00:00" );
    statusBarLabelTime->setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );
    statusBarLabelTime->setToolTip( tr ( "Recording time" ) );

    statusBarLabelFps = new QLabel();
    statusBarLabelFps->setText( "0" );
    statusBarLabelFps->setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );
    statusBarLabelFps->setToolTip( tr( "Actual frames per second" ) );

    statusBarLabelSize = new QLabel();
    statusBarLabelSize->setText( "0" );
    statusBarLabelSize->setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );
    statusBarLabelSize->setToolTip( tr( "Size in KB" ) );
    
    statusbarLabelScreenSize = new QLabel();
    statusbarLabelScreenSize->setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );
    statusbarLabelScreenSize->setToolTip( tr( "Recording screensize" ) );

    statusBarLabelCodec = new QLabel();
    statusBarLabelCodec->setText( VideocodecComboBox->currentText() );
    statusBarLabelCodec->setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );
    statusBarLabelCodec->setToolTip( tr( "Codec" ) );
    
    statusBarLabelFormat = new QLabel();
    statusBarLabelFormat->setText( VideoContainerComboBox->currentText() );
    statusBarLabelFormat->setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );
    statusBarLabelFormat->setToolTip( tr( "Format" ) );

    statusBarLabelAudio = new QLabel();
    statusBarLabelAudio->setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );
    statusBarLabelAudio->setToolTip( tr( "Audio" ) );
    
    statusBarLabelFpsSettings = new QLabel();
    statusBarLabelFpsSettings->setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );
    statusBarLabelFpsSettings->setToolTip( tr( "Settings fps" ) );

    QStatusBar *statusBar = new QStatusBar;
    setStatusBar( statusBar );

    //qDebug() << statusBar->parent();
    
    QLabel * LabelTemp = new QLabel();
    statusBar->addWidget( LabelTemp, 120 );
    
    statusBar->addWidget( statusBarProgForRecord, 0 );
    statusBar->addWidget( statusBarLabelTime, 0);
    statusBar->addWidget( statusBarLabelFps, 0 );
    statusBar->addWidget( statusBarLabelSize, 0 );
    statusBar->addWidget( statusbarLabelScreenSize, 0 );
    
    statusBar->addWidget( statusBarLabelCodec, 0 );
    statusBar->addWidget( statusBarLabelFormat, 0 );
    statusBar->addWidget( statusBarLabelAudio, 0 );
    statusBar->addWidget( statusBarLabelFpsSettings, 0 );
    
    QLabel * LabelTemp1 = new QLabel();
    statusBar->addWidget( LabelTemp1, 40 );
    
    qfont = statusBar->font();
    qfont.setPixelSize( 12 );
    statusBar->setFont( qfont );
    
    qDebug() << "[vokoscreen]" << "---Begin search Videoplayer---";
    QStringList playerList = QStringList()  << "kaffeine"
                                            << "vlc"
                                            << "gnome-mplayer"
                                            << "ffplay"
					    << "avplay"
                                            << "totem"
                                            << "pia"
                                            << "xine"
                                            << "gxine"
                                            << "gmplayer"
                                            << "kplayer"
                                            << "smplayer"
                                            << "smplayer2"
                                            << "dragon"
                                            << "banshee";
    playerList.sort();
    QString playerName;
    QString resultString( qgetenv( "PATH" ) );
    QStringList pathList = resultString.split( ":" );
    for ( int x = 0; x < playerList.size(); ++x )
     {
       for ( int i = 0; i < pathList.size(); ++i )
       {
         playerName = pathList.at( i );
         playerName = playerName.append( QDir::separator() ).append( playerList.at( x ) );
         if ( QFile::exists( playerName ) )
         {
           qDebug() << "[vokoscreen]" << "Find Videoplayer :" << playerName;
           VideoplayerComboBox->addItem( playerList.at( x ), playerName );
           break;
         }
       }
     }
     
     qDebug() << "[vokoscreen]" << "---End search Videoplayer---";
     qDebug();

    // Einstellungen aus .conf einlesen
    QSettings settings( ProgName, ProgName );

    // Dient nur zum anlegen des Profils damit ffmpeglog erstellt werden kann
    settings.beginGroup("vokoscreen");
      settings.setValue("Version", Version);
    settings.endGroup();

    settings.beginGroup( "Audio" );
      AudioOnOffCheckbox->setCheckState( Qt::CheckState( settings.value( "AudioOnOff", 2 ).toUInt() ) );
    settings.endGroup();

    settings.beginGroup("Alsa" );
      AlsaRadioButton->setChecked( settings.value( "Alsa", false ).toBool() );
    settings.endGroup();

    settings.beginGroup("Pulse");
      PulseDeviceRadioButton->setChecked( settings.value( "Pulse", true ).toBool() );
    settings.endGroup();

    settings.beginGroup("Record");
      FullScreenRadioButton->setChecked( settings.value( "FullScreen", true ).toBool() );
      WindowRadioButton->setChecked( settings.value( "Window", false ).toBool() );
      AreaRadioButton->setChecked( settings.value( "Area", false ).toBool() );
    settings.endGroup();

    settings.beginGroup( "Miscellaneous" );
      QDir Dir;
      if ( Dir.exists( settings.value( "VideoPath", PathMoviesLocation() ).toString() ) )
        SaveVideoPathLineEdit->setText( settings.value( "VideoPath", PathMoviesLocation() ).toString() );
      else
	PathMoviesLocation();

      int x = VideoplayerComboBox->findText( settings.value( "Videoplayer" ).toString(),Qt::MatchExactly );
      if ( x == -1 )
        VideoplayerComboBox->setCurrentIndex( 0 );
      else
        VideoplayerComboBox->setCurrentIndex( x );
      
      MinimizedCheckBox->setCheckState( Qt::CheckState( settings.value( "Minimized", 0 ).toUInt() ) );
      
      CountdownSpinBox->setValue(  settings.value( "Countdown", 0 ).toUInt() );
    settings.endGroup();
    
    settings.beginGroup( "Videooptions" );
      FrameSpinBox->setValue( settings.value( "Frames", 25 ).toInt() );
      VideocodecComboBox->setCurrentIndex( VideocodecComboBox->findText( settings.value( "Videocodec", "mpeg4" ).toString() ) );
      VideoContainerComboBox->setCurrentIndex( VideoContainerComboBox->findText( settings.value( "Format", "mkv" ).toString() ) );
      HideMouseCheckbox->setCheckState( Qt::CheckState( settings.value( "HideMouse").toUInt() ) );
    settings.endGroup();

    settings.beginGroup( "GUI" );
      setGeometry( settings.value( "X", "100" ).toUInt(), settings.value( "Y", "100" ).toUInt(), width(), height() );
      tabWidget->setCurrentIndex ( settings.value( "Tab", "0" ).toUInt() );
    settings.endGroup();
    
    // Statusbar
    stateChangedAudio( AudioOnOffCheckbox->checkState() );
      statusBarLabelFpsSettings->setText( QString::number( FrameSpinBox->value() ) );
    
    SystemCall = new QProcess( this );

    connect( AudioOnOffCheckbox,     SIGNAL( clicked() ), SLOT( AudioOnOff() ) );
    connect( AlsaRadioButton,        SIGNAL( clicked() ), SLOT( AudioOnOff() ) );
    connect( PulseDeviceRadioButton, SIGNAL( clicked() ), SLOT( AudioOnOff() ) );
    
    connect( SystemCall, SIGNAL( stateChanged ( QProcess::ProcessState) ),this, SLOT( stateChanged( QProcess::ProcessState) ) );
    connect( SystemCall, SIGNAL( error( QProcess::ProcessError) ),        this, SLOT( error( QProcess::ProcessError) ) );
    connect( SystemCall, SIGNAL( readyReadStandardError() ),              this, SLOT( readyReadStandardError() ) );
    connect( SystemCall, SIGNAL( readyReadStandardOutput() ),             this, SLOT( readyReadStandardOutput() ) );

    windowMoveTimer = new QTimer( this );
    connect( windowMoveTimer, SIGNAL( timeout() ), this, SLOT( windowMove() ) );

    // Area ein-ausblenden wenn Radiobutton immer wieder angecklickt wird
    connect( AreaRadioButton,       SIGNAL( clicked() ), SLOT( AreaOnOff() ) );
    connect( FullScreenRadioButton, SIGNAL( clicked() ), SLOT( AreaOnOff() ) );
    connect( WindowRadioButton,     SIGNAL( clicked() ), SLOT( AreaOnOff() ) );
    settings.beginGroup( "Area" );
       myregionselection = new regionselection();
       myregionselection->setGeometry ( settings.value( "X", 200 ).toUInt(),
       				        settings.value( "Y", 200 ).toUInt(),
				        settings.value( "Width", 200 ).toUInt() + myregionselection->borderLeft + myregionselection->borderRight + myregionselection->frameWidth,
				        settings.value( "Height", 200 ).toUInt() + myregionselection->borderTop + myregionselection->borderBottom + myregionselection->frameWidth
				      );
       myregionselection->close();
    settings.endGroup();
    
    connect( MagnifierCheckBox, SIGNAL( clicked() ), SLOT( showMagnifier() ) );
    magnifier = new QvkMagnifier();
    magnifier->close();

    connect( magnifier, SIGNAL( closeMagnifier() ), SLOT( uncheckMagnifier() ) );
    
    // Clean vokoscreen temp
    QDir dir( PathTempLocation() );
    QStringList stringList = dir.entryList( QDir::Files, QDir::Time | QDir::Reversed );
    for ( int i = 0; i < stringList.size(); ++i )
      dir.remove( PathTempLocation().append( QDir::separator() ).append( stringList.at( i ) ) );

   makeAsoundrc();
   AudioOnOff();
   pulseUnloadModule();

   // Abfrage einbauen ob Systemtray existiert

   SystemTrayIconGreen = new QSystemTrayIcon( QIcon ( ":/pictures/start.png" ) );
   SystemTrayIconGreen->show();
   SystemTrayIconGreen->setToolTip( tr( "Start" ) );
   
   SystemTrayIconRed = new QSystemTrayIcon( QIcon ( ":/pictures/stop.png" ) );
   SystemTrayIconRed->hide();
   SystemTrayIconRed->setToolTip( tr( "Stop" ) );
   
   SystemTrayIconYellow = new QSystemTrayIcon( QIcon ( ":/pictures/pause.png" ) );
   SystemTrayIconYellow->hide();
   SystemTrayIconYellow->setToolTip( tr( "Pause" ) );

   SystemTrayIconBlue = new QSystemTrayIcon( QIcon ( ":/pictures/go.png" ) );
   SystemTrayIconBlue->hide();
   SystemTrayIconBlue->setToolTip( tr( "Go" ) );

   connect( SystemTrayIconGreen,  SIGNAL( activated ( QSystemTrayIcon::ActivationReason ) ), this, SLOT( SystemTrayStart( QSystemTrayIcon::ActivationReason ) ) );
   connect( SystemTrayIconRed,    SIGNAL( activated ( QSystemTrayIcon::ActivationReason ) ), this, SLOT( SystemTrayStop( QSystemTrayIcon::ActivationReason ) ) );
   connect( SystemTrayIconYellow, SIGNAL( activated ( QSystemTrayIcon::ActivationReason ) ), this, SLOT( SystemTrayPause( QSystemTrayIcon::ActivationReason ) ) );
   connect( SystemTrayIconBlue,   SIGNAL( activated ( QSystemTrayIcon::ActivationReason ) ), this, SLOT( SystemTrayGo( QSystemTrayIcon::ActivationReason ) ) ); 
   
   shortcutMagnifier = new QxtGlobalShortcut( this );
   connect( shortcutMagnifier, SIGNAL( activated() ), this, SLOT( ShortcutMagnifier() ) );
   shortcutMagnifier->setShortcut( QKeySequence( "Ctrl+Shift+F9" ) );

   shortcutStart = new QxtGlobalShortcut( this );
   connect( shortcutStart, SIGNAL( activated() ), this, SLOT( preRecord() ) );
   shortcutStart->setShortcut( QKeySequence( "Ctrl+Shift+F10" ) );
   
   shortcutStop = new QxtGlobalShortcut( this );
   connect( shortcutStop, SIGNAL( activated() ), this, SLOT( Stop() ) );
   shortcutStop->setShortcut( QKeySequence( "Ctrl+Shift+F11" ) );
   shortcutStop->setEnabled( false );
   
   shortcutPause = new QxtGlobalShortcut( this );
   connect( shortcutPause, SIGNAL( activated() ), this, SLOT( ShortcutPause() ) );
   shortcutPause->setShortcut( QKeySequence( "Ctrl+Shift+F12" ) );
   
   QvkAlsaWatcher * myAlsaWatcher = new QvkAlsaWatcher();
   connect( myAlsaWatcher, SIGNAL( changed( QStringList ) ), this, SLOT( AlsaWatcherEvent( QStringList ) ) );

   QFileSystemWatcher * VideoFileSystemWatcher = new QFileSystemWatcher();
   VideoFileSystemWatcher->addPath( SaveVideoPathLineEdit->displayText() );
   connect( VideoFileSystemWatcher, SIGNAL( directoryChanged( const QString& ) ), this, SLOT( myVideoFileSystemWatcher( const QString ) ) );
   myVideoFileSystemWatcher( "" );
   
   clickedScreenSize();
   AreaOnOff();
   
}


screencast::~screencast()
{
}


void screencast::send()
{
  QvkMail *vkMail = new QvkMail( this );
  (void)vkMail;
}


void screencast::clickedRecordButtonScreenSize()
{
  if ( FullScreenRadioButton->isChecked() )
    statusbarLabelScreenSize->setText( "F:" + getRecordWidth() + "x" + getRecordHeight() );
  
  if ( WindowRadioButton->isChecked() )
    statusbarLabelScreenSize->setText( "W:" + getRecordWidth() + "x" + getRecordHeight() );
  
  if ( AreaRadioButton->isChecked() )
    statusbarLabelScreenSize->setText( "A:" + getRecordWidth() + "x" + getRecordHeight() );
}


/**
 * Statusbar
 */
void screencast::clickedScreenSize()
{
  if ( FullScreenRadioButton->isChecked() )
    statusbarLabelScreenSize->setText( "F" );
  
  if ( WindowRadioButton->isChecked() )
    statusbarLabelScreenSize->setText( "W" );
  
  if ( AreaRadioButton->isChecked() )
    statusbarLabelScreenSize->setText( "A" );
}


/**
 * Statusbar
 */
void screencast::valueChangedFrames( int i ) 
{
  (void)i;
  statusBarLabelFpsSettings->setText( QString::number( FrameSpinBox->value() ) );
}
  

/**
 * Statusbar
 */
void screencast::stateChangedAudio( int state )
{
  if ( state == Qt::Unchecked )
     statusBarLabelAudio->setText( "off" );

  if ( state == Qt::Checked )
  {
     if ( AlsaRadioButton->isChecked() )    
       statusBarLabelAudio->setText( "Alsa" );
     
     if ( PulseDeviceRadioButton->isChecked() )
       statusBarLabelAudio->setText( "Pulse" );
  }
}


/**
 * Statusbar
 */
void screencast::clickedAudioAlsa( bool checked ) 
{
  if ( checked )
     statusBarLabelAudio->setText( "Alsa" );
}


/**
 * Statusbar
 */
void screencast::clickedAudioPulse( bool checked )
{
  if ( checked == true ) 
    statusBarLabelAudio->setText( "Pulse" );
}


/**
 * Statusbar
 */
void screencast::currentIndexChangedCodec( int index )
{
  (void)index;
  statusBarLabelCodec->setText( VideocodecComboBox->currentText() );  
}


/**
 * Statusbar
 */
void screencast::currentIndexChangedFormat( int index )
{
 (void)index;
 statusBarLabelFormat->setText( VideoContainerComboBox->currentText() );
}


void screencast::MagnifierDialog()
{
  magnifier->getDialogMagnifier( this );
}


void screencast::myVideoFileSystemWatcher( const QString & path )
{
  (void)path;
  QDir Dira( PathMoviesLocation() );
  QStringList filters;
  filters << "vokoscreen*";
  QStringList List = Dira.entryList( filters, QDir::Files, QDir::Time );
  
  if ( List.isEmpty() )
  {
    PlayButton->setEnabled( false );
    sendPushButton->setEnabled( false );
  }
  else
  {
    PlayButton->setEnabled( true );
    sendPushButton->setEnabled( true );
  }
}


/*
QString boolToStr( bool boo )
{
  return ( ( true == boo ) ? "true" : "false" );
}
*/


/**
 * CardxList beinhaltet "card0", "card1" ...
 * */
void screencast::AlsaWatcherEvent( QStringList CardxList )
{
  qDebug() << "[vokoscreen] ---Begin search Alsa capture device---";

  AlsaHwComboBox->clear();
  AlsaDeviceList.clear();
  // Für jede card wird eine Instanz erzeugt und in AlsaDeviceList abgelegt
  for( int i = 0; i <= CardxList.count() - 1; i++ )
  {
    QvkAlsaDevice * alsaDevice = new QvkAlsaDevice( CardxList[ i ] );
    AlsaDeviceList.append( alsaDevice );
    AlsaHwComboBox->addItem( AlsaDeviceList.at( i )->getAlsaName() , i );
  }

  QSettings settings( ProgName, ProgName );
  settings.beginGroup( "Alsa" );
    int x = AlsaHwComboBox->findText( settings.value( "NameCaptureCard" ).toString(),Qt::MatchExactly );
    AlsaHwComboBox->setCurrentIndex( x );
  settings.endGroup();
  qDebug() << "[vokoscreen] ---End search Alsa capture device---";
  qDebug();

  settings.beginGroup( "Pulse" );
  PulseMultipleChoice();

  for ( int x = 0; x < 10; x++ )
      for ( int i = 0; i < getPulseInputDevicesCount(); i++ )
      {
          QCheckBox *aa = getCheckBoxPulseDevice( i );
          if ( aa->text() == settings.value( "NameCaptureCard-" + QString::number( x + 1 ) ).toString() )
              aa->setCheckState( Qt::Checked );
      }
  settings.endGroup();
}


void screencast::WindowMinimized()
{
  setWindowState( Qt::WindowMinimized );
}


/**
 * Wird beim beenden von vokoscreen aufgerufen
 */
void screencast::closeEvent( QCloseEvent * event )
{
  (void)event;
  Stop();
  saveSettings();
  myregionselection->close();
  magnifier->close();
  if ( webcamCheckBox->isVisible()  )
     webcamCheckBox->webcamClose();
}


void screencast::saveSettings()
{
  QSettings settings( ProgName, ProgName );
  
  settings.clear();

  settings.beginGroup( "vokoscreen" );
    settings.setValue( "Version", Version );
  settings.endGroup();

  settings.beginGroup( "Audio" );
    settings.setValue( "AudioOnOff", AudioOnOffCheckbox->checkState() );
  settings.endGroup();

  settings.beginGroup( "Alsa" );
    settings.setValue( "Alsa", AlsaRadioButton->isChecked() );
    settings.setValue( "NameCaptureCard", AlsaHwComboBox->currentText() );
  settings.endGroup();

  settings.beginGroup( "Pulse" );
    settings.setValue( "Pulse", PulseDeviceRadioButton->isChecked() );
    for ( int i = 1; i < getCountCheckedPulseDevices() + 1; i++ )
      settings.setValue( "NameCaptureCard-" + QString::number( i ), getPulseDeviceName( i ) );
  settings.endGroup();

  settings.beginGroup( "Record" );
    settings.setValue( "FullScreen", FullScreenRadioButton->isChecked() );
    settings.setValue( "Window", WindowRadioButton->isChecked() );
    settings.setValue( "Area", AreaRadioButton->isChecked() );
  settings.endGroup();

  settings.beginGroup( "Miscellaneous" );
    settings.setValue( "VideoPath", SaveVideoPathLineEdit->displayText() );
    settings.setValue( "Videoplayer", VideoplayerComboBox->currentText() );
    settings.setValue( "Minimized", MinimizedCheckBox->checkState() );
    settings.setValue( "Countdown", CountdownSpinBox->value() );
  settings.endGroup();

  settings.beginGroup( "Videooptions" );
    settings.setValue( "Frames", FrameSpinBox->value() );
    settings.setValue( "Videocodec", VideocodecComboBox->currentText() );
    settings.setValue( "Format", VideoContainerComboBox->currentText() );
    settings.setValue( "HideMouse", HideMouseCheckbox->checkState() );    
  settings.endGroup();
  
  settings.beginGroup( "GUI" );
    settings.setValue( "X", x() );
    settings.setValue( "Y", y() );
    settings.setValue( "Tab", tabWidget->currentIndex() );
  settings.endGroup();
  
  settings.beginGroup( "Area" );
    settings.setValue( "X", myregionselection->getX() );
    settings.setValue( "Y", myregionselection->getY() );
    settings.setValue( "Width", myregionselection->getWidth() );
    settings.setValue( "Height", myregionselection->getHeight() );
  settings.endGroup();
  
  webcamCheckBox->saveSettings();
  myregionselection->saveSettings();
}


void screencast::ShortcutMagnifier()
{
  MagnifierCheckBox->click();
}


void screencast::showMagnifier()
{
  if ( MagnifierCheckBox->isChecked() )
    magnifier->show();
  else
    magnifier->close(); 
}


void screencast::uncheckMagnifier()
{
  if ( MagnifierCheckBox->checkState() == Qt::Checked )
    MagnifierCheckBox->click();
}


void screencast::ShortcutPause()
{
  PauseButton->click();
}


void screencast::SystemTrayStart( QSystemTrayIcon::ActivationReason reason )
{
  if ( reason == QSystemTrayIcon::Trigger )
  {
    SystemTrayIconGreen->hide();
    recordButton->click();
  }
}


void screencast::SystemTrayStop( QSystemTrayIcon::ActivationReason reason )
{
  if ( reason == QSystemTrayIcon::Trigger )
    StopButton->click();
}


void screencast::SystemTrayPause( QSystemTrayIcon::ActivationReason reason )
{
  if ( reason == QSystemTrayIcon::Trigger )
    PauseButton->click();
}


void screencast::SystemTrayGo( QSystemTrayIcon::ActivationReason reason )
{
  if ( reason == QSystemTrayIcon::Trigger )
    PauseButton->click();
}


void screencast::AreaOnOff()
{
  if ( FullScreenRadioButton->isChecked() or WindowRadioButton->isChecked() )
    myregionselection->close();

  if ( AreaRadioButton->isChecked() )
  {
    myregionselection->close();
    myregionselection->show();
  }
}


/**
 * Search program foo in PATH
 */
QString getFileWithPath( QString ProgName )
{
    QString find;
    QString prog;
    QString resultString( qgetenv( "PATH" ) );
    QStringList pathList = resultString.split( ":" );
    for ( int i = 0; i < pathList.size(); ++i )
    {
      prog = pathList.at( i ) + QDir::separator() + ProgName;
      if ( QFile::exists( prog ) )
      {
        find = prog;
        break;
      }
    }
    return find;
}


/**
 * Looking for external programs
 */
void screencast::searchExternalPrograms()
{
  qDebug() << "[vokoscreen]" << "---Begin Search external tools---";
/*  
  if ( needProgram("ffmpeg") )
     qDebug() << "[vokoscreen]" << "Find ffmpeg";
  else
     qDebug() << "[vokoscreen]" << "Error: ffmpeg is not found, this is an ffmpeg tool. Please install ffmpeg";
*/
    QString Prog = "ffmpeg";
    QFileInfo info( getFileWithPath( Prog ) );
    if ( info.isSymLink() )
    {
      QString fileName = info.symLinkTarget();  
      QFileInfo fileInfo( fileName );
      qDebug() << "[vokoscreen]" << getFileWithPath( Prog ) << "is a link and pointing to"<< fileName;
      if ( fileInfo.baseName() == "avconv" )
      {
        recordApplikation = "avconv";
	qDebug() << "[vokoscreen] use avconv";
      }
    }
    else
    {
     if ( needProgram( "ffmpeg" ) )
     {
        recordApplikation = "ffmpeg";
        qDebug() << "[vokoscreen]" << "Find ffmpeg";
     }
     else
     {
       if ( needProgram( "avconv" ) )
       {
          recordApplikation = "avconv";
          qDebug() << "[vokoscreen]" << "Find avconv";
       }
     }
    }
  
  
  
  
  if ( needProgram("pactl") )
     qDebug() << "[vokoscreen]" << "Find pactl";
  else
     qDebug() << "[vokoscreen]" << "Error: pactl is not found, this is an PulseAudio-utils tool. Please install pactl";
  
  if ( needProgram("mkvmerge") )
     qDebug() << "[vokoscreen]" << "Find mkvmerge";
  else
     qDebug() << "[vokoscreen]" << "Error: mkvmerge is not found, this is an mkvtoolnix tool. Please install mkvmerge";
  
  qDebug() << "[vokoscreen]" << "---End search external tools---";
  qDebug();
}


/**
 * Set standard video codec and options
 */
void screencast::setVideocodecStandardComboBox()
{
  VideocodecComboBox->setCurrentIndex( VideocodecComboBox->findText( "mpeg4", Qt::MatchExactly ) );
  VideoContainerComboBox->setCurrentIndex( VideoContainerComboBox->findText( "mkv", Qt::MatchExactly ) );
}


/**
 * Set standard fps
 */
void screencast::setFrameStandardSpinbox()
{
  FrameSpinBox->setValue( 25 );
}


/**
 * Erstellt ~/.asoundrc wenn nicht im Userhome vorhanden
 * -Die .asoundrc sollte nur erstellt werden wenn der Start von ffmpeg fehlschägt.
 * Hint: Abfrage muß noch eingebaut werden.
 */
void screencast::makeAsoundrc()
{
  QString homeLocation = QDesktopServices::storageLocation( QDesktopServices:: QDesktopServices::HomeLocation );
  QString asoundrc = homeLocation + "/.asoundrc";
  
  qDebug() << "[vokoscreen]" << "---Begin search PulseAudio Plugin---";
  QFile qFile( asoundrc );
  if ( not qFile.exists() )
  {
    qFile.open( QIODevice::WriteOnly );
    qDebug() << "[vokoscreen] File not found .asound for PulseAudio Plugin";
    qDebug() << "[vokoscreen] Create file .asound for PulseAudio Plugin";
    QTextStream out( &qFile );
    out << "# create by vokoscreen " << Version << "\n";
    out << "\n";
    out << "pcm.pulse {\n";
    out << "  type pulse\n";
    out << "}\n";
    out << "\n";
    out << "ctl.pulse {\n";
    out << "  type pulse\n";
    out << "}\n";
    out << "\n";
    qFile.flush();
    qFile.close();
  }
  else
    qDebug() << "[vokoscreen] Found file .asound for PulseAudio Plugin";
  
  qDebug() << "[vokoscreen]" << "---End search PulseAudio Plugin---";
  qDebug();

}


void screencast::saveVideoPath()
{
  QString dir = QFileDialog::getExistingDirectory( this, tr( "Open Directory" ),
                QDesktopServices::storageLocation( QDesktopServices::HomeLocation ), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks );

  if ( dir > "" )
    SaveVideoPathLineEdit->setText( dir );
}


void screencast::readyReadStandardError()
{
  int s = beginTime.secsTo( QDateTime::currentDateTime() );
  int HH = s / 3600;
  int MM = ( s- ( s / 3600 ) * 3600 ) / 60;
  int SS = s % 60;
  QTime myTime( HH, MM, SS);
  QString time = myTime.toString ( "hh:mm:ss");
  statusBarLabelTime->setText( time );

  QString output = SystemCall->readAllStandardError();
  if ( output.contains( "fps=", Qt::CaseInsensitive ) )
  {
    int x = output.indexOf( "fps" );
    statusBarLabelFps->setText( output.mid( x + 4, 3 ).replace( " ", "" ) );
  }

  // 
  int fps = statusBarLabelFps->text().toUInt();
  if ( fps >=  FrameSpinBox->value() )
  {
    statusBarLabelFps->setToolTip( tr( "Actual frames per second" ) );
    statusBarLabelFps->setAutoFillBackground( true );
    QPalette pal( statusBarLabelFps->palette() );
    pal.setColor( QPalette::Foreground, Qt::black );
    pal.setColor( QPalette::Background, Qt::green );
    statusBarLabelFps->setPalette( pal );
  }

  if ( fps < FrameSpinBox->value() )
  {
    statusBarLabelFps->setToolTip( tr( "Your Actual frames in the Settings is too heigh" ) );
    statusBarLabelFps->setAutoFillBackground( true );
    QPalette pal( statusBarLabelFps->palette() );
    if ( pal.color( QPalette::Background ) == Qt::red )
    {
      pal.setColor( QPalette::Foreground, Qt::black );
      pal.setColor( QPalette::Background, Qt::yellow );
    }
    else
    {
      pal.setColor( QPalette::Foreground, Qt::black );
      pal.setColor( QPalette::Background, Qt::red );
    }
    statusBarLabelFps->setPalette( pal );
  }    
  
  QFileInfo fileInfo( PathTempLocation() + QDir::separator() + nameInMoviesLocation );
  statusBarLabelSize->setText( QString::number( fileInfo.size() / 1024 ) );
}


void screencast::readyReadStandardOutput()
{
//   qDebug() << SystemCall->readAllStandardOutput();
}


/**
 * Je nach Prozessstaus von ffmpeg werden die Widget enabled bzw. disabled
 */
void screencast::stateChanged ( QProcess::ProcessState newState )
{
    if ( ( newState == QProcess::Running ) and ( pause == false) )
    {
      StopButton->setEnabled(true);
      PauseButton->setEnabled(true);
      recordButton->setEnabled(false);
      PlayButton->setEnabled(false);
      FullScreenRadioButton->setEnabled( false );
      WindowRadioButton->setEnabled( false );
      AreaRadioButton->setEnabled( false );
      
      TabWidgetAudioFrame->setEnabled(false);
      TabWidgetMiscellaneousFrame->setEnabled(false);
      TabWidgetVideoOptionFrame->setEnabled( false );

      SystemTrayIconGreen->hide();
      SystemTrayIconYellow->show();      
      SystemTrayIconRed->show();
      SystemTrayIconBlue->hide();
    }
  
    if ((newState == QProcess::NotRunning) and (pause == false))
    {
      StopButton->setEnabled(false);
      PauseButton->setEnabled(false);
      recordButton->setEnabled(true);
      FullScreenRadioButton->setEnabled( true );
      WindowRadioButton->setEnabled( true );
      AreaRadioButton->setEnabled( true );
      
      TabWidgetAudioFrame->setEnabled(true);
      TabWidgetMiscellaneousFrame->setEnabled(true);
      TabWidgetVideoOptionFrame->setEnabled( true );
      
      SystemTrayIconGreen->show();
      SystemTrayIconYellow->hide();
      SystemTrayIconRed->hide();      
      SystemTrayIconBlue->hide();
      
      if ( MagnifierCheckBox->isChecked() )
	MagnifierCheckBox->click();
    }
    
    if ((newState == QProcess::NotRunning) and (pause == true) and ( PauseButton->isChecked() ))
    {
      StopButton->setEnabled(false);
      PauseButton->setEnabled(true);
      recordButton->setEnabled(false);
      PlayButton->setEnabled(false);
      FullScreenRadioButton->setEnabled( false );
      WindowRadioButton->setEnabled( false );
      AreaRadioButton->setEnabled( false );
      
      TabWidgetAudioFrame->setEnabled(false);
      TabWidgetMiscellaneousFrame->setEnabled(false);
      TabWidgetVideoOptionFrame->setEnabled( false );

      SystemTrayIconGreen->hide();
      SystemTrayIconRed->hide();
      SystemTrayIconYellow->hide();
      SystemTrayIconBlue->show();
    }

    if ((newState == QProcess::Running) and (pause == true) and ( not PauseButton->isChecked() ))
    {
      StopButton->setEnabled(true);
      PauseButton->setEnabled(true);
      recordButton->setEnabled(false);
      PlayButton->setEnabled(false);
      FullScreenRadioButton->setEnabled( false );
      WindowRadioButton->setEnabled( false );
      AreaRadioButton->setEnabled( false );
      
      TabWidgetAudioFrame->setEnabled(false);
      TabWidgetMiscellaneousFrame->setEnabled(false);
      TabWidgetVideoOptionFrame->setEnabled( false );
      
      SystemTrayIconGreen->hide();
      SystemTrayIconYellow->show();      
      SystemTrayIconRed->show();
      SystemTrayIconBlue->hide();
    }
      
    if ( ( newState == QProcess::NotRunning ) and ( pause == true ) and ( not PauseButton->isChecked() ) )
    {
      StopButton->setEnabled(false);
      PauseButton->setEnabled(false);
      recordButton->setEnabled(true);
      FullScreenRadioButton->setEnabled( true );
      WindowRadioButton->setEnabled( true );
      AreaRadioButton->setEnabled( true );
      
      TabWidgetAudioFrame->setEnabled(true);
      TabWidgetMiscellaneousFrame->setEnabled(true);
      TabWidgetVideoOptionFrame->setEnabled( true );
      
      SystemTrayIconGreen->show();
      SystemTrayIconRed->hide();
      SystemTrayIconYellow->hide();
      SystemTrayIconBlue->hide();
    }
       
    if (newState == QProcess::Running)
    {
      qDebug();
      qDebug() << "[vokoscreen] ffmpeg is running and is ready for reading and writing";
    }
   
    if (newState == QProcess::NotRunning)
    {
      qDebug();
      qDebug() << "[vokoscreen] ffmpeg is not running";

      //Enables the customarea rectangle again. (Is diabled in record() )
        if(!PauseButton->isChecked()){
            myregionselection->lockFrame(false);
        }
    }
}


void screencast::error( QProcess::ProcessError error )
{
  // Kann mann testen in dem man den ffmpeg aus der Processliste tötet
  if ( error == QProcess::Crashed )
  {
    // remove crashed directory
    QSettings settings1( ProgName, ProgName );
    QFileInfo settingsPath1( settings1.fileName() );
    QFile file1( settingsPath1.absolutePath() );
    QString workDirectory1 = file1.fileName();
    QDir Dir1( "" );
    Dir1.setCurrent( workDirectory1 );
    if ( Dir1.cd( "crashed" ) )
    {
      QStringList stringList1 = Dir1.entryList( QDir::Files, QDir::NoSort );
      for ( int i = 0; i < stringList1.count(); ++i )
        Dir1.remove( stringList1[ i ] );
    }
    
    // Move ffmpeg.log, vokoscreen.log, vokoscreen.conf in directory crashed
    QSettings settings( ProgName, ProgName );
    QFileInfo settingsPath( settings.fileName() );
    QFile file( settingsPath.absolutePath() );
    QString workDirectory = file.fileName();
    QDir Dir( "" );
    Dir.setCurrent( workDirectory );
    Dir.mkdir( "crashed" );
    
    QStringList stringList = Dir.entryList( QDir::Files, QDir::NoSort );
    for ( int i = 0; i < stringList.count(); ++i )
      QFile::copy( workDirectory + QDir::separator() + stringList[ i ], workDirectory + QDir::separator() + "crashed" + QDir::separator() + stringList[ i ]);
    
    QString crashedtext = "[vokoscreen] ffmpeg is crashed on ";
    QDateTime dateTime = QDateTime::currentDateTime();
    QString time = dateTime.toString( "hh:mm" );
    QString day = dateTime.toString( "dddd" );
    qDebug();
    qDebug() << "********************************************************";
    qDebug() << crashedtext + day + ", " + time;
    qDebug() << "********************************************************";
    QMessageBox msgBox;
    msgBox.setIcon( QMessageBox::Critical );
    msgBox.setText( crashedtext + day + ", " + time );
    msgBox.exec();
  }

  // Noch nicht getestet
  if ( error == QProcess::FailedToStart )
  {
    qDebug() << "Der Prozess konnte nicht gestartet werden. Entweder ist das aufgerufene Programm nicht installiert, oder der ffmpeg Aufruf ist Fehlerhaft oder Sie haben nicht über ausreichende Berechtigungen um das Programm aufzurufen.";
    QMessageBox msgBox;
    msgBox.setText("Der Prozess konnte nicht gestartet werden. Entweder ist das aufgerufene Programm nicht installiert, oder der ffmpeg Aufruf ist Fehlerhaft oder Sie haben nicht über ausreichende Berechtigungen um das Programm aufzurufen.");
    msgBox.exec();
  }
}


/**
 * Search program foo in PATH
 */
bool screencast::needProgram( QString ProgName )
{
    bool find = false;
    QString prog;
    QString resultString( qgetenv( "PATH" ) );
    QStringList pathList = resultString.split( ":" );
      for ( int i = 0; i < pathList.size(); ++i )
      {
        prog = pathList.at( i ) + QDir::separator() + ProgName;
        if ( QFile::exists( prog ) )
        {
          find = true;
          break;
        }
      }
    return find;
}


void screencast::moveWindowPause()
{
  pause = true;
  PauseButton->setChecked( true );
  PauseButton->setText( tr( "Go" ) );
  SystemCall->terminate();
  SystemCall->waitForFinished();
  pulseUnloadModule();
}


void screencast::moveWindowGo()
{
  PauseButton->setChecked( false );  
  PauseButton->setText( tr ( "Pause" ) );
  startRecord( PathTempLocation() + QDir::separator() + PauseNameInTmpLocation() );
}


void screencast::Pause()
{
  if ( FullScreenRadioButton->isChecked() or AreaRadioButton->isChecked() )
  {
    pause = true;
    if ( PauseButton->isChecked() )
    {
      shortcutStop->setEnabled( false );
      windowMoveTimer->stop();
      PauseButton->setText( tr ( "Go" ) );
      SystemCall->terminate();
      SystemCall->waitForFinished();
      pulseUnloadModule();
    }
    else
    {
      QVariant aa = AlsaHwComboBox->itemData( AlsaHwComboBox->currentIndex() );
      QvkAlsaDevice *inBox = AlsaDeviceList.at( aa.toInt() );
      if ( inBox->isbusy() and AlsaRadioButton->isChecked() )
      {
        QMessageBox msgBox;
        QString message;
        message.append( tr( "Device %1 is busy" ).arg( inBox->getAlsaHw() ) );
	msgBox.setText( message );
        msgBox.exec();
	PauseButton->click();
        return;
      }
      
      shortcutStop->setEnabled( true );
      PauseButton->setText( tr( "Pause" ) );
      startRecord( PathTempLocation() + QDir::separator() + PauseNameInTmpLocation() );
    }
  }
  
  
  if ( WindowRadioButton->isChecked() )
  {
    pause = true;
    if ( PauseButton->isChecked() )
    {
      shortcutStop->setEnabled( false );
      windowMoveTimer->stop();
      PauseButton->setText( tr ( "Go" ) );
      SystemCall->terminate();
      SystemCall->waitForFinished();
      pulseUnloadModule();
    }
    else
    {
      QVariant aa = AlsaHwComboBox->itemData( AlsaHwComboBox->currentIndex() );
      QvkAlsaDevice *inBox = AlsaDeviceList.at( aa.toInt() );
      if ( inBox->isbusy() and AlsaRadioButton->isChecked() )
      {
        QMessageBox msgBox;
        QString message;
        message.append( tr( "Device %1 is busy" ).arg( inBox->getAlsaHw() ) );
        msgBox.setText( message );
        msgBox.exec();
	PauseButton->click();
        return;
      }
      shortcutStop->setEnabled( true );
      PauseButton->setText( tr( "Pause" ) );
      startRecord( PathTempLocation() + QDir::separator() + PauseNameInTmpLocation() );
      windowMoveTimer->start();
    }
  }
}


/**
 * Play last screencast with Videoplayer
 */
void screencast::play()
{
  if ( MagnifierCheckBox->isChecked() )
	MagnifierCheckBox->click();
 
  QVariant aa = VideoplayerComboBox->itemData( VideoplayerComboBox->currentIndex() ); // get userdata from ComboBox
  QString player = aa.toString();
  player = player.replace( "\n", "" ); 
  
  QDir Dira( PathMoviesLocation() );
  QStringList filters;
  filters << "vokoscreen*";
  QStringList List = Dira.entryList( filters, QDir::Files, QDir::Time );
  if ( List.isEmpty() )
  {
    QProcess *SystemCall = new QProcess( this );
    SystemCall->start( player );
  }
  else
  {
    QProcess *SystemCall = new QProcess( this );
    SystemCall->start( player + " " + PathMoviesLocation() + QDir::separator() + List.at( 0 ) );
  }
}


/**
 * Versionsnummer von ffmpeg aufbereiten so diese mit "kleiner gleich" bzw. "größer gleich" ausgewertet werden kann
 */
QString screencast::getFfmpegVersion()
{
  QProcess Process;
  Process.start("ffmpeg -version");
  Process.waitForFinished();
  QString ffmpegversion = Process.readAllStandardOutput();
  Process.close();
  
  QStringList list = ffmpegversion.split( "\n" );
  ffmpegversion = list[ 0 ];
  ffmpegversion.replace(QRegExp( "[A-Z a-z ]" ), "" );
 
  // major
  list.clear();
  list = ffmpegversion.split( "." );

  if ( list.count() == 1 )
    list.append( "00" );
    
  if ( list.count() == 2 )
    list.append( "00" );
  
  // major
  QString major;
  if ( list[ 0 ].size() < 2 )
    major = list[ 0 ].prepend( "0" );
  else
    major = list[ 0 ];
  
  //minor
  QString minor;
  if ( list.count() > 1 )
  {
    if ( list[ 1 ].size() < 2 )
      minor = list[ 1 ].prepend( "0" );
    else
      minor = list[ 1 ];
  }
  
  //patch
  QString patch;
  if ( list.count() > 2 )
  {
    if ( list[ 2 ].size() < 2 ) 
     patch = list[ 2 ].prepend( "0" ); 
    else
      patch = list[ 2 ];
  }

  return major + "." + minor + "." + patch;
}


void screencast::windowMove()
{
  // Begin Window is closed
  QStringList stringList;
  QList<WId> list = QxtWindowSystem::windows() ;
  for( int i = 0; i < list.count(); i++)
    stringList << QString::number( list[ i ] );

  if ( !stringList.contains( QString::number( moveWindowID ) ) )
  {
    windowMoveTimer->stop();
    StopButton->click();
    return;
  } 
  // End Window is closed
  
  QString x = QString::number( QxtWindowSystem::windowGeometry( moveWindowID ).x() + leftFrameBorder );
  QString y = QString::number( QxtWindowSystem::windowGeometry( moveWindowID ).y() + topFrameBorder );
  if ( ( deltaXMove != x ) or ( deltaYMove != y ) )
    if ( SystemCall->state() == QProcess::Running ) 
      moveWindowPause();
  
  if ( ( deltaXMove == x ) and ( deltaYMove == y ) )
    if ( ( SystemCall->state() == QProcess::NotRunning ) )
    {
      QStringList result = ffmpegString.split( ":0." );
      QString str1 = result[ 0 ];
      QString str2 = result[ 1 ];
      result.clear();
      result = str2.split( " " );
      result[ 0 ] = ":0.0+" + x + "," + y;
      str2 = "";
      for ( int i = 0; i < result.count(); i++ )
        str2.append( result.at( i ) + " " );

      ffmpegString = str1 + str2.trimmed() + " ";
      moveWindowGo();
    }

  deltaXMove = x;
  deltaYMove = y; 
}


/**
 * 
 */
int screencast::getPulseInputDevicesCount()
{
  QProcess *Process = new QProcess( this );
  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
  env.insert("LANG", "c");
  Process->setProcessEnvironment(env);
  Process->start( "pactl list" );
  Process->waitForFinished();
  QString output = Process->readAllStandardOutput();
  Process->close();
  delete Process;
  
  QStringList list = output.split( "\n" );

  QStringList result = list.filter( "Source #", Qt::CaseInsensitive );
  return result.count();
}


/**
 * 
 */
QString screencast::getPulseInputName( int value )
{
  QProcess *Process = new QProcess( this );
  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
  env.insert("LANG", "c");
  Process->setProcessEnvironment(env);
  Process->start( "pactl list" );
  Process->waitForFinished();
  QString output = QString::fromUtf8(Process->readAllStandardOutput());
  Process->close();
  delete Process;
  
  QStringList list = output.split( "\n" );

  QStringList result = list.filter( "Source #", Qt::CaseInsensitive );
  QString SourceNummber = result[ value - 1 ];
  
  int index = list.indexOf( SourceNummber );
  QString name = list[ index + 3 ];
  QStringList resultList = name.split( ":" );
  name = resultList[ 1 ].trimmed();
  return name;
}


/**
 * Get Pulse Devicname
 */
QString screencast::getPulseInputDevices( int value )
{
  QProcess *Process = new QProcess( this );
  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
  env.insert("LANG", "c");
  Process->setProcessEnvironment(env);
  Process->start( "pactl list" );
  Process->waitForFinished();
  QString output = Process->readAllStandardOutput();
  Process->close();
  delete Process;
 
  QStringList list = output.split( "\n" );

  QStringList result = list.filter( "Source #", Qt::CaseInsensitive );
  QString SourceNummber = result[ value - 1 ];

  int index = list.indexOf( SourceNummber );
  QString device = list[ index + 2 ];
  QStringList resultList = device.split( ":" );
  device = resultList[ 1 ].trimmed();
  return device;
}


/**
 * Get Pulse Samplerate
 */
QString screencast::getPulseSample( int value )
{
  QProcess *Process = new QProcess( this );
  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
  env.insert("LANG", "c");
  Process->setProcessEnvironment(env);
  Process->start( "pactl list" );
  Process->waitForFinished();
  QString output = Process->readAllStandardOutput();
  Process->close();
  delete Process;
  
  QStringList list = output.split( "\n" );
  
  QStringList result = list.filter( "Source #", Qt::CaseInsensitive );
  QString SourceNummber = result[ value - 1 ];
  
  int index = list.indexOf( SourceNummber );
  QString device = list[ index + 5 ];
  QStringList resultList = device.split( ":" );
 
  list.clear();
  QString aa = resultList[ 1 ].trimmed();
  list = aa.split( " " );
  
  result.clear();
  result = list.filter( "Hz", Qt::CaseInsensitive );

  QString sample = result[ 0 ].replace("Hz","");
  return sample;
}


/**
 * Englisch:
 * Return Samplerate from checked Device
 * Pulse: The lowest rate sampler for the selected device is used
 * 
 * German:
 * Gibt Samplerrate von ausgewählten Geräten zurück.
 * Pulse: Wenn mehrere Geräte ausgewählt wurden wird die niedrigste Samplerrate herangezogen
 */
QString screencast::mySample()
{
  QString ret = "48000";
  
  if ( PulseDeviceRadioButton->isChecked() )
  {
    QList<QCheckBox *> listQFrame = Pulseframe->findChildren<QCheckBox *>();
    QCheckBox *inBox;
    QString Sample;

    QStringList list;
    for ( int i = 0; i < listQFrame.count(); i++ )
    {
      inBox = listQFrame.at( i );
      if ( inBox->checkState() == Qt::Checked  )
        list.append( getPulseSample( i + 1 ) );
    }
  
    list.sort();

    if ( not list.empty() )
      ret = list.first();
  }
   
  if ( AlsaRadioButton->isChecked() )
  {
    // Aus der ComboBox das ausgewählte Device holen
    QVariant aa = AlsaHwComboBox->itemData( AlsaHwComboBox->currentIndex() );
    QvkAlsaDevice *inBox = AlsaDeviceList.at( aa.toInt() );
    ret = inBox->getAlsaSample();
  }
    
   return ret;
}


void screencast::AudioOnOff()
{
  if ( AudioOnOffCheckbox->checkState() == Qt::Checked )
  {
    AlsaRadioButton->setEnabled( true );
    PulseDeviceRadioButton->setEnabled( true );
    
    if ( PulseDeviceRadioButton->isChecked() )
      Pulseframe->setEnabled( true );
    else
      Pulseframe->setEnabled( false );
    
    if ( AlsaRadioButton->isChecked() )
      AlsaHwComboBox->setEnabled( true );
    else
      AlsaHwComboBox->setEnabled( false );
  }
  else
  {
    AlsaRadioButton->setEnabled( false );
    AlsaHwComboBox->setEnabled( false );
//    PulseRadioButton->setEnabled( false );
    Pulseframe->setEnabled( false );
    PulseDeviceRadioButton->setEnabled( false );
  }
}


/**
 * Erstellt eine Scrollarea mit einem Frame
 * in dem die Checkboxen gesetzt werden
 * 
 * In setAccessibleName steht das Pulse Device
 */
void screencast::PulseMultipleChoice()
{
  qDebug() << "[vokoscreen]" << "---Begin search PulseAudio Capture Devices---";
  
  QList<QScrollArea *> listQScrollArea = TabWidgetAudioFrame->findChildren<QScrollArea *>();
  
  if ( listQScrollArea.count() > 0 )
  {
      delete scrollAreaPulse;
      listQScrollArea.clear();
  }
  
  if ( listQScrollArea.count() == 0 )
  {
      Pulseframe = new QFrame;

      scrollAreaPulse = new QScrollArea(TabWidgetAudioFrame);
      scrollAreaPulse->setWidget(Pulseframe);
      scrollAreaPulse->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
      scrollAreaPulse->setWidgetResizable(true);

      gridAudio->addWidget(scrollAreaPulse, 0, 1);

      QVBoxLayout *vboxPulses = new QVBoxLayout(Pulseframe);

      for ( int i = 0; i < getPulseInputDevicesCount(); ++i )
      {
          namePulse = new QCheckBox;
          namePulse->setText( getPulseInputName( i + 1 ) );
          namePulse->setAccessibleName( getPulseInputDevices( i + 1 ) );
          namePulse->setToolTip( tr ( "Select one or more devices" ) );
          vboxPulses->addWidget(namePulse);
          qDebug() << "[vokoscreen]" << "Find CaptureCard:" << namePulse->text() << "with device:" << namePulse->accessibleName();
      }
  }
  
  AudioOnOff();
  
  qDebug() << "[vokoscreen]" << "---End search PulseAudio Capture Devices---";
  qDebug();
}


QString screencast::PathMoviesLocation()
{
  QString Path;
  if ( SaveVideoPathLineEdit->displayText() > "" )
     Path = SaveVideoPathLineEdit->displayText();
  else
  {
    if ( QDesktopServices::storageLocation( QDesktopServices::MoviesLocation).isEmpty() )
    {
       Path = QDesktopServices::storageLocation( QDesktopServices:: QDesktopServices::HomeLocation );
       SaveVideoPathLineEdit->setText(Path);
    }
    else
    {
      Path = QDesktopServices::storageLocation( QDesktopServices::MoviesLocation );
      SaveVideoPathLineEdit->setText( Path );
    }
  }
  return Path;
}


QString screencast::PathTempLocation()
{
  // Doppelter ProgName um Eindeutigkeit in tmp zuerreichen
  QString tmpName = ProgName + "-" + ProgName;
  QString tempPathProg = QDesktopServices::storageLocation ( QDesktopServices::TempLocation ) + QDir::separator() + tmpName;
  QDir dirTempPathProg( tempPathProg );
  if ( not dirTempPathProg.exists() )
  {
      QString tempPath = QDesktopServices::storageLocation ( QDesktopServices::TempLocation );
      QDir dirTempPath( tempPath );
      dirTempPath.mkdir( tmpName );
  }
  return tempPathProg;  
}


/**
 * Return the new screencastname
 */
QString screencast::NameInMoviesLocation()
{
  return "vokoscreen-" + QDateTime::currentDateTime().toString( "yyyy-MM-dd_hh-mm-ss" ) + "." + VideoContainerComboBox->currentText();
}


/**
 * Return the new pausename
 */
QString screencast::PauseNameInTmpLocation()
{
  QString myFilename = "screencast-pause";
  QString myFilenameExtension = "." + VideoContainerComboBox->currentText();
  QString myName = PathTempLocation() + QDir::separator() + myFilename + myFilenameExtension;

  QFile *myFile = new QFile( myName );
  uint x = 0;
  do
  {
    x++;
    myFile->setFileName( PathTempLocation() + QDir::separator() + myFilename + "-" + QString().number( x ) + myFilenameExtension );
    myName = myFile->fileName();
  } while ( myFile->exists() );

  return myFilename + "-" + QString().number( x ) + myFilenameExtension;
}


QString screencast::myAlsa()
{
  QString value;
  if ( AudioOnOffCheckbox->checkState() == Qt::Checked )
  {
    if ( AlsaRadioButton->isChecked() )  
    {
      QVariant aa = AlsaHwComboBox->itemData( AlsaHwComboBox->currentIndex() );
      QvkAlsaDevice *inBox = AlsaDeviceList.at( aa.toInt() );
      if ( AlsaHwComboBox->currentIndex() > -1 )
        value = "-f alsa -ac " + inBox->getChannel() + " -i " + inBox->getAlsaHw() + " ";
      else
        value = "";
    }
    
    if ( PulseDeviceRadioButton->isChecked() )      
    {
      QCheckBox *box;
      int counter = 0;
      QList<QCheckBox *> listQFrame = Pulseframe->findChildren<QCheckBox *>();
      if ( listQFrame.count() > 0 )
        for ( int i = 0; i < listQFrame.count(); i++ )
        {
          box = listQFrame.at( i );
          if ( box->checkState() == Qt::Checked  )
            counter++;
        }

      if ( counter > 0 )
        value = "-f alsa -i pulse ";
      else
	value = "";
    }

  }
  else
      value = "";

  return value;
}


void screencast::pulseUnloadModule()
{
  qDebug() << "";
  qDebug() << "[vokoscreen] ---Begin Pulse unload Module---";
  QProcess Process;
  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
  env.insert("LANG", "c");
  Process.setProcessEnvironment(env);
  Process.start( "pactl list" );
  Process.waitForFinished();
  QString output = Process.readAllStandardOutput();
  Process.close();
  
  QStringList list = output.split( "\n" );
  
  QStringList modulNumberList;
  QString str;
  QString strLog;
  for ( int i = 0; i < list.count(); i++ )
  {
     str = list[ i ];
     if ( str.contains("vokoscreenMix", Qt::CaseInsensitive) )
     {
       str = list[ i - 2 ];
       if ( str.contains("Module", Qt::CaseInsensitive) )
       {
	 str.replace("Module #", "");
	 str.replace("\n", "");
	 modulNumberList.insert ( 0, str );
       }
     }
  }
  
  for ( int i = 0; i < modulNumberList.count(); i++ )
  {
    Process.start("pactl unload-module " + modulNumberList[ i ] );
    Process.waitForFinished();
    Process.close();
    qDebug() << "[vokoscreen] pactl unload-module number" << modulNumberList[ i ] ;
  }
  qDebug() << "[vokoscreen] ---End Pulse unload Module---";
  qDebug() << "";
}


/**
 * Returns Checkbox from Pulse device
 * 
 */
QCheckBox * screencast::getCheckBoxPulseDevice( int value )
{
  QList<QCheckBox *> listQFrame = Pulseframe->findChildren<QCheckBox *>();  
  QCheckBox *inBox;  
  inBox = listQFrame.at( value );
  return inBox;
}


/**
 * Returns xte checked pulse devicename
 */
QString screencast::getPulseDeviceName( int value )
{
  QList<QCheckBox *> listQFrame = Pulseframe->findChildren<QCheckBox *>();
  QCheckBox *inBox;
  int x = 1;
  QString name;
  
  for ( int i = 0; i < listQFrame.count(); i++ )
  {
    inBox = listQFrame.at( i );
    if ( inBox->checkState() == Qt::Checked  )
    {
      if  ( x == value )
        name = inBox->text();
      x++;
    }
  }
  return name;
}


/**
 *  Returns count checked pulse devices
 */
int screencast::getCountCheckedPulseDevices()
{
  QList<QCheckBox *> listQFrame = Pulseframe->findChildren<QCheckBox *>();
  QCheckBox *inBox;
  int x = 0;
  
  for ( int i = 0; i < listQFrame.count(); i++ )
  {
    inBox = listQFrame.at( i );
    if ( inBox->checkState() == Qt::Checked  )
      x++;
  }
  return x;
}


/**
 * Return checked Pulse Device
 */
const QString screencast::myPulseDevice()
{
  QList<QCheckBox *> listQFrame = Pulseframe->findChildren<QCheckBox *>();
  QCheckBox *box;
  QList<int> integerList;
  QString ret;
  
  for ( int i = 0; i < listQFrame.count(); i++ )
  {
    box = listQFrame.at( i );
    if ( box->checkState() == Qt::Checked  )
      integerList.append( i );
  }

  if ( integerList.count() == 0 )
    ret = "";

  if ( integerList.count() == 1 )
  {
    box = listQFrame[ integerList[ 0 ] ];
    ret = box->accessibleName();
  }

  if ( integerList.count() > 1 )
    ret = "vokoscreenMix.monitor";

  return ret;
}


QString screencast::myAcodec()
{
  QString acodec;
  if ( ( AudioOnOffCheckbox->checkState() == Qt::Checked ) and ( AlsaRadioButton->isChecked() ) and ( AlsaHwComboBox->currentText() > "" ) )
     //return " -acodec libvorbis";
     return " -acodec libmp3lame";

  
  if ( ( AudioOnOffCheckbox->checkState() == Qt::Checked ) and ( PulseDeviceRadioButton->isChecked() ) and ( myPulseDevice() > "" ) )
     //return " -acodec libvorbis";
     return " -acodec libmp3lame";
 
  return "";
}


QString screencast::noMouse()
{
  if ( HideMouseCheckbox->checkState() == Qt::Checked  )
    return "+nomouse";
  else
    return "";
}


void screencast::preRecord()
{
  if ( AlsaRadioButton->isChecked() and AudioOnOffCheckbox->isChecked() )
  {
    QVariant aa = AlsaHwComboBox->itemData( AlsaHwComboBox->currentIndex() );
    QvkAlsaDevice *inBox = AlsaDeviceList.at( aa.toInt() );
    if ( inBox->isbusy() )
    {
      QMessageBox msgBox;
      msgBox.setIcon( QMessageBox::Information );
      QString message;
      message.append( tr( "Device %1 is busy" ).arg( inBox->getAlsaHw() ) );
      msgBox.setText( message );
      msgBox.exec();
      return;
    }
    else
    {
      // Kanäle werden kurz vor der Aufnahme ermittelt
      inBox->setChannel();
    }
  }
  
  if ( WindowRadioButton->isChecked() )
    if ( firststartWininfo == false )
    {
      vkWinInfo = new QvkWinInfo();
      connect( vkWinInfo, SIGNAL( windowChanged() ), this, SLOT( record() ) );
      return;
    }
   record();
}


void screencast::setRecordWidth( QString value )
{
  screenRecordWidth = value; 
}


QString screencast::getRecordWidth()
{
  return screenRecordWidth; 
}

void screencast::setRecordHeight( QString value )
{
  screenRecordHeight = value; 
}


QString screencast::getRecordHeight()
{
  return screenRecordHeight; 
}


void screencast::Countdown()
{
  if ( CountdownSpinBox->value() > 0 )
  {
    recordButton->setEnabled( false );

    QDesktopWidget *desk = QApplication::desktop();
    int Width = 250;
    int Height = 250;;
    int x = ( desk->screenGeometry().width() / 2 ) - ( Width / 2 );
    int y = ( desk->screenGeometry().height() / 2 ) -( Height / 2 );
    
    QFrame * countdownDialog = new QFrame();
    countdownDialog->setGeometry( x, y, Width, Height );
    countdownDialog->setWindowFlags( Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint );
    if( QX11Info::isCompositingManagerRunning() )
       countdownDialog->setAttribute( Qt::WA_TranslucentBackground, true );
    
    countdownDialog->show();
    
    QFont qfont;;
    qfont.setPixelSize( 200 );
    
    QLabel * label = new QLabel( countdownDialog );
    label->setGeometry( 0, 0, Width, Height );
    label->setAlignment( Qt::AlignCenter );    
    countdownDialog->setFont( qfont );
    label->show();
    
    QPalette pal( label->palette() );
    pal.setColor( QPalette::Foreground, Qt::red);
    label->setPalette( pal );
    
    for ( int i = CountdownSpinBox->value(); i >= 1; i-- )
    {
      label->setText( "" );
      QCoreApplication::processEvents( QEventLoop::AllEvents );     

      label->setText( QString::number( i ) );
      
      
      QCoreApplication::processEvents( QEventLoop::AllEvents );     
      label->update();
      for ( int ii = 1; ii <= 10; ii++)
      {
        QCoreApplication::processEvents( QEventLoop::AllEvents );     
        QTest::qSleep( 100 );
      }
    }

    // Der Desktopanimation "Langsames ausblenden" entgegenwirken
    label->setText( "" );
    QCoreApplication::processEvents( QEventLoop::AllEvents );     
    label->hide();
    QCoreApplication::processEvents( QEventLoop::AllEvents );     
    countdownDialog->close();
  } 
}


void screencast::record()
{
  
  Countdown();
  
  shortcutStart->setEnabled( false );
  shortcutStop->setEnabled( true );
   
  if ( MinimizedCheckBox->checkState() == Qt::Checked )
    WindowMinimized();
  
  if ( FullScreenRadioButton->isChecked() )
  {
    QDesktopWidget *desk = QApplication::desktop();
    setRecordWidth( tr( "%1" ).arg( desk->screenGeometry().width() ) );
    setRecordHeight( tr( "%1" ).arg( desk->screenGeometry().height() ) );
  }

  QString deltaX = "0";
  QString deltaY = "0";
  
  if ( WindowRadioButton->isChecked() )
    if ( firststartWininfo == false )
    {
       
/*      
      TestFrame->setWindowFlags( Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint );
      //QCoreApplication::processEvents( QEventLoop::AllEvents );     
      TestFrame->show();
      //TestFrame->hide();
      qDebug() << "X():" << TestFrame->x() << "Y():" << TestFrame->y();
      qDebug() << "geometry.x()" << TestFrame->geometry().x() << "geometry.y()" << TestFrame->geometry().y();
      qDebug() << "frameGeometry.width()" << TestFrame->frameGeometry() << "frameGeometry.height()" << TestFrame->frameGeometry().height();
*/      
      int rightFrameBorder = 0;
      int bottomFrameBorder = 0;
      
      // Begin ermitteln der Fensterrahmen
      //qDebug() << "with Frame :" << frameGeometry().width() << "x" << frameGeometry().height();
      //qDebug() << "without Frame:" << geometry().width() << "x" << geometry().height();
      
      leftFrameBorder = geometry().x() - x();
      //qDebug() << "left frame:" << leftFrameBorder;
      
      rightFrameBorder = frameGeometry().width() - geometry().width() - leftFrameBorder;
      //qDebug() << "right frame:" << rightFrameBorder;
      
      topFrameBorder = geometry().y() - y();
      //qDebug() << "top frame:" << topFrameBorder;
      
      bottomFrameBorder = frameGeometry().height() - geometry().height() - topFrameBorder;
      //qDebug() << "bottom frame:" << bottomFrameBorder;
      // End  ermitteln der Fensterrahmen
      
      setRecordWidth( QString::number( vkWinInfo->width().toUInt() - leftFrameBorder - rightFrameBorder) );
      setRecordHeight( QString::number( vkWinInfo->height().toUInt() - topFrameBorder - bottomFrameBorder ) );
      
      QDesktopWidget *desk = QApplication::desktop();
      if ( getRecordWidth().toUInt() >= QString::number( desk->screenGeometry().width() ).toUInt() )
        deltaX = "0";
      else
        deltaX  = QString::number( vkWinInfo->x().toUInt() + leftFrameBorder );
      //qDebug() << "deltaX:" << deltaX;
      
      deltaY  = QString::number( QxtWindowSystem::windowGeometry( vkWinInfo->getWinID() ).y() + topFrameBorder );
      //qDebug() << "deltaY:" << deltaY;

      moveWindowID = vkWinInfo->getWinID();
	
      deltaXMove = deltaX;
      deltaYMove = deltaY;

      windowMoveTimer->start( 120 );
      firststartWininfo = true;
    }
  
  if ( AreaRadioButton->isChecked() )
  {
    setRecordWidth( QString().number( myregionselection->getWidth() ) );
    setRecordHeight( QString().number( myregionselection->getHeight() ) );

//    deltaX  = QString().number( myregionselection->getX() );
//    deltaY  = QString().number( myregionselection->getY() );
    deltaX  = QString().number( myregionselection->getXRecordArea() );
    deltaY  = QString().number( myregionselection->getYRecordArea() );

    // 
    myregionselection->cleanRecordArea( true );
    
    //Makes the rectangle unmovable and unresizeable (Is enabled yet again when process finished)
    myregionselection->lockFrame( true );
  }
  
  // -report wird erst ab ffmpeg version 0.9 unterstützt
  QString myReport = "";
  if ( recordApplikation == "ffmpeg" )
  {
    if ( getFfmpegVersion() >= "00.09.00" )
      myReport = "-report ";
    else
      myReport = "";

    qDebug() << "[vokoscreen]" << "Report :" << myReport;
  }

  // set working directory for writing and delete the ffmpegLog from Profil directory
  QSettings settings( ProgName, ProgName );
  QFileInfo settingsPath( settings.fileName() );
  QFile file( settingsPath.absolutePath() );
  QString workDirectory = file.fileName();
  QDir Dir( "" );
  Dir.setCurrent( workDirectory );  
  
  // bestehendes ffmpegLog löschen
  QDir Dira( "" );
  QStringList filters;
  filters << "ffmpeg*";
  QStringList List = Dira.entryList(filters, QDir::Files, QDir::Name );
  QFile FfmpegFile;
  for (int i = 0; i < List.size(); ++i)
     FfmpegFile.remove( List.at(i) );

  // evtl. müßte hier erst mal nachgeschaut werden ob Temp leer ist.
  // Clean vokoscreen temp 
  QDir dir( PathTempLocation() );
  QStringList stringList = dir.entryList( QDir::Files, QDir::Time | QDir::Reversed );
  for ( int i = 0; i < stringList.size(); ++i )
     dir.remove( PathTempLocation().append(QDir::separator() ).append(stringList.at( i ) ) );

  // frame rate
  QString frame = "-r " + QString().number( FrameSpinBox->value() );
  
  // Videocodec
  QString myVcodec = VideocodecComboBox->currentText();
  if ( myVcodec == "libx264" )
  {
    // Number of pixels must be divisible by two
    int intRecordX = getRecordWidth().toInt();
    if ( ( intRecordX % 2 ) == 1 )
      setRecordWidth( QString().number( --intRecordX ) );

    // Number of pixels must be divisible by two
    int intRecordY = getRecordHeight().toInt();
    if ( ( intRecordY % 2 ) == 1 )
      setRecordHeight( QString().number( --intRecordY ) );
    
    myVcodec = "libx264 -preset veryfast"; // Standard
    //myVcodec = "libx264 -preset medium";
    //myVcodec = "libx264 -preset veryslow";
    
  }  

  nameInMoviesLocation = NameInMoviesLocation();

  QString quality;
  if ( ( getFfmpegVersion() < "01.01.00" ) and ( recordApplikation == "ffmpeg" ) )
    quality = " -sameq ";
  else
    quality = " -qscale 1 ";

  clickedRecordButtonScreenSize();
  
//  ffmpegString = "/usr/bin/ffmpeg "
  ffmpegString = recordApplikation + " "
               + myReport
               + myAlsa()
	       + "-f x11grab "
               + frame
	       + " -s "
	       + getRecordWidth()
	       + "x"
	       + getRecordHeight()
	       + " -i :0.0+"
	       + deltaX + "," 
	       + deltaY
	       + " -pix_fmt yuv420p" // Neu das funktioniert sehr gut, ohne ca. 80 fps, mit 99fps bei eingestellten 99fps und preset medium aber nur unter opensuse, unter Ubuntu 13.04 keine Veränderung
               + noMouse()
	       + " -vcodec "
	       + myVcodec
       	       + myAcodec()
	       //+ " -ab 256k "  //********************************************Neu
      	       //+ " -ar 48000 "  //********************************************Neu
      	       + " -ar " + mySample()
	       + quality
	       + frame + " ";
	       	            
  startRecord( PathTempLocation() + QDir::separator() + nameInMoviesLocation );
  
  QFile FileVokoscreenLog(settingsPath.absolutePath() + QDir::separator() + ProgName + ".log");
  if ( !FileVokoscreenLog.open( QIODevice::WriteOnly ) ) 
    qDebug() << "Datei konnte nicht angelegt werden: " << FileVokoscreenLog.errorString();
  
  QTextStream stream( &FileVokoscreenLog );
  stream << ProgName << " Version: " << Version << "\n";
  stream << "Record resolution: " << getRecordWidth() << "x" << getRecordHeight() << "\n";
  stream << "Alsa string: " << myAlsa() << "\n";
  stream << "Qt Version: " << qVersion() << "\n";
  stream << "ffmpeg Version: " << getFfmpegVersion() << "\n";
  stream << "Record String: " << ffmpegString << PathMoviesLocation() << QDir::separator() << nameInMoviesLocation << "\n";
}


void screencast::startRecord( QString RecordPathName )
{
  qDebug() << "[vokoscreen]"<< "ffmpegcommand :" << ffmpegString + RecordPathName;

  if ( PulseDeviceRadioButton->isChecked() )
  {
    QProcess Process;
    QString value = myPulseDevice();
    if ( value == "vokoscreenMix.monitor" )
    {
      Process.start("pactl load-module module-null-sink sink_name=vokoscreenMix");
      Process.waitForFinished();
      QString modulNumber = Process.readAllStandardOutput();
      Process.close();
      qDebug();
      qDebug() << "[vokoscreen] ---Begin Pulse loade module---";
      modulNumber.replace("\n", "");    
      qDebug() << "[vokoscreen] pactl load-module module-null-sink sink_name=vokoscreenMix " << modulNumber;
    
      QList<QCheckBox *> listQFrame = Pulseframe->findChildren<QCheckBox *>();
      QCheckBox *box;
      QList<int> integerList;
      for ( int i = 0; i < listQFrame.count(); i++ )
      {
        box = listQFrame.at( i );
        if ( box->checkState() == Qt::Checked  )
          integerList.append( i );
      }
      
      for ( int i = 0; i < integerList.count(); i++ )
      {
        box = listQFrame[ integerList[ i ] ];
        Process.start("pactl load-module module-loopback source=" + box->accessibleName() + " sink=vokoscreenMix");
        Process.waitForFinished();
        QString modulNumber = Process.readAllStandardOutput();
        modulNumber.replace("\n", "");
        Process.close();
        qDebug() << "[vokoscreen] pactl load-module module-loopback source=" << box->accessibleName() << "sink=vokoscreenMix number" << modulNumber;
      }
      qDebug() << "[vokoscreen] ---End Pulse loade module---";
      qDebug();
    }
  
    if ( value > "" )
    {
      QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
      env.insert( "PULSE_SOURCE", value );
      SystemCall->setProcessEnvironment( env );
    }
  }
  
  SystemCall->start( ffmpegString + RecordPathName );

  // Recordtime Statusbar
  beginTime  = QDateTime::currentDateTime();

}


void screencast::Stop()
{
  
  myregionselection->cleanRecordArea( false );
  
  shortcutStart->setEnabled( true );
  shortcutStop->setEnabled( false );
  
  if ( SystemCall->state() == QProcess::Running )
  {
    SystemCall->terminate();
    SystemCall->waitForFinished();
  }
  
  if ( pause )
  {
    QString mergeString = "/usr/bin/mkvmerge ";
    QDir dir( PathTempLocation() );
    QStringList stringList = dir.entryList(QDir::Files, QDir::Time | QDir::Reversed);
    for ( int i = 0; i < stringList.size(); ++i )
      if ( i == 0 )    
        mergeString.append( PathTempLocation() ).append( QDir::separator() ).append( stringList.at( i ) );
      else
        mergeString.append( " + " ).append( PathTempLocation() ).append( QDir::separator() ).append( stringList.at( i ) );

    mergeString.append( " -o " ).append( PathMoviesLocation() + QDir::separator() + nameInMoviesLocation );
    SystemCall->start( mergeString );
    SystemCall->waitForFinished();
    for ( int i = 0; i < stringList.size(); ++i )
      dir.remove( PathTempLocation().append( QDir::separator() ).append( stringList.at( i ) ) );

    qDebug() << "[vokoscreen]" << "Mergestring :" << mergeString;
  }
  else
  {
    QString FileInTemp = PathTempLocation() + QDir::separator() + nameInMoviesLocation;
    QFile::copy ( FileInTemp, PathMoviesLocation() + QDir::separator() + nameInMoviesLocation );
    QFile::remove ( FileInTemp );
  }
  
  QDir dir;
  dir.rmdir( PathTempLocation() );
  
  pause = false;
  windowMoveTimer->stop();
  firststartWininfo = false;

  pulseUnloadModule();
  
}

//#include "screencast.moc"
