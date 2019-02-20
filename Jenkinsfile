pipeline {
  agent any
  stages {
    stage('Build Code') {
      steps {
        echo 'Building...'
        bat 'build.bat'
      }
    }
  }
  environment {
    path = 'c:\\devkitPro\\msys2\\usr\\bin\\;c:\\windows\\system32'
    DEVKITARM = 'C:\\devkitPro\\devkitARM'
    DEVKITPRO = 'C:\\devkitPro'
  }
}