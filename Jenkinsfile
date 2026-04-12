pipeline {
    agent any

    options {
        timestamps()
        timeout(time: 15, unit: 'MINUTES')
        disableConcurrentBuilds()
    }

    environment {
        REPORTS = "reports"
        BUILD_DIR = "build"
    }

    stages {

        stage('Precheck Tools') {
            steps {
                bat '''
                    echo === Precheck Tools ===

                    python --version || exit /b 1
                    where python || exit /b 1

                    where gcc || echo WARNING: gcc NOT FOUND
                    where make || echo WARNING: make NOT FOUND
                    where cppcheck || echo WARNING: cppcheck NOT FOUND
                '''
            }
        }

        stage('Prepare Workspace') {
            steps {
                bat '''
                    echo === Preparing workspace ===

                    if not exist %REPORTS% mkdir %REPORTS%
                    if not exist %REPORTS%\\coverage mkdir %REPORTS%\\coverage
                    if not exist %BUILD_DIR% mkdir %BUILD_DIR%
                '''
            }
        }

        stage('Static Analysis - cppcheck') {
            steps {
                bat '''
                    echo === Static Analysis (cppcheck) ===

                    where cppcheck >nul 2>nul
                    if %ERRORLEVEL% NEQ 0 (
                        echo ERROR: cppcheck not installed
                        exit /b 1
                    )

                    cppcheck --enable=all ^
                             --inconclusive ^
                             --xml --xml-version=2 ^
                             --force ^
                             -I include ^
                             src 2> %REPORTS%\\cppcheck.xml
                '''
            }
        }

        stage('Build') {
            steps {
                bat '''
                    echo === Build ===

                    where make >nul 2>nul
                    if %ERRORLEVEL% NEQ 0 (
                        echo ERROR: make not found
                        exit /b 1
                    )

                    make clean
                    make all -j4
                '''
            }
        }

        stage('Unit Tests') {
            steps {
                bat '''
                    echo === Unit Tests ===

                    gcc -Wall -Wextra -fprofile-arcs -ftest-coverage ^
                        -I include ^
                        -o %BUILD_DIR%\\test_core ^
                        tests\\test_main.c ^
                        src\\core\\*.c ^
                        unity\\unity.c

                    if not exist %BUILD_DIR%\\test_core.exe (
                        echo ERROR: test binary not built
                        exit /b 1
                    )

                    %BUILD_DIR%\\test_core.exe > %REPORTS%\\unity_output.txt

                    python scripts\\unity_to_junit.py ^
                        %REPORTS%\\unity_output.txt ^
                        %REPORTS%\\junit.xml
                '''
            }
            post {
                always {
                    junit allowEmptyResults: true,
                          testResults: "${env.REPORTS}/junit.xml"
                }
            }
        }

        stage('Coverage (Optional)') {
            steps {
                bat '''
                    echo === Coverage ===

                    where lcov >nul 2>nul
                    if %ERRORLEVEL% NEQ 0 (
                        echo WARNING: lcov not installed - skipping coverage
                        exit /b 0
                    )

                    lcov --capture --directory . --output-file %REPORTS%\\coverage.info
                    genhtml %REPORTS%\\coverage.info --output-directory %REPORTS%\\coverage
                '''
            }
            post {
                always {
                    publishHTML(target: [
                        reportDir: "${env.REPORTS}/coverage",
                        reportFiles: "index.html",
                        reportName: "Coverage Report",
                        keepAll: true,
                        allowMissing: true
                    ])
                }
            }
        }
    }

    post {
        always {
            archiveArtifacts artifacts: 'reports/**', allowEmptyArchive: true
            echo "Pipeline finished"
        }

        success {
            echo "✅ SUCCESS"
        }

        failure {
            echo "❌ FAILED - check logs"
        }
    }
}
