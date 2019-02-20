pipeline {
  agent any
  stages {
    stage('Build Code') {
      steps {
        bat(script: 'build.bat', returnStdout: true)
      }
    }
  }
  environment {
    path = 'c:\\devkitPro\\msys2\\usr\\bin\\'
  }
}