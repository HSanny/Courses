" ATP project vim script: Mon Dec 08, 2014 at 03:33 AM -0600.

let b:atp_MainFile = 'homework.tex'
let g:atp_mapNn = 0
let b:atp_autex = 1
let b:atp_TexCompiler = 'pdflatex'
let b:atp_TexOptions = '-synctex=1'
let b:atp_TexFlavor = 'tex'
let b:atp_auruns = '1'
let b:atp_ReloadOnError = '1'
let b:atp_OutDir = '/Users/JimmyLin/workspace/Courses/COMP3600_Theory_Of_Computation/Assignment01'
let b:atp_OpenViewer = '1'
let b:atp_XpdfServer = 'homework'
let b:atp_Viewer = 'open'
let b:TreeOfFiles = {}
let b:ListOfFiles = []
let b:TypeDict = {}
let b:LevelDict = {}
let b:atp_BibCompiler = 'bibtex'
let b:atp_StarEnvDefault = ''
let b:atp_StarMathEnvDefault = ''
let b:atp_updatetime_insert = 4000
let b:atp_updatetime_normal = 2000
let b:atp_LocalCommands = ['\hmwkTitle', '\hmwkDueDate', '\hmwkClass', '\hmwkClassTime', '\hmwkClassInstructor', '\hmwkAuthorName', '\enterProblemHeader{', '\exitProblemHeader{', '\labelAnswer', '\homeworkProblemName', '\problemAnswer', '\problemLAnswer', '\homeworkSectionName', '\sectionAnswer']
let b:atp_LocalEnvironments = ['homeworkProblem', 'homeworkSection']
let b:atp_LocalColors = []
