pipeline {
    agent any

    environment {
        REPORTS = "reports"
        BUILD_DIR = "build"
    }

    stages {

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

        stage('Check Tools') {
            steps {
                bat '''
                    echo === Checking tools ===

                    where gcc || echo WARNING: gcc NOT FOUND
                    where make || echo WARNING: make NOT FOUND
                    where cppcheck || echo WARNING: cppcheck NOT FOUND
                    where python || echo WARNING: python NOT FOUND
                '''
            }
        }

        stage('Static Analysis - cppcheck') {
            steps {
                bat '''
                    echo === Static Analysis (cppcheck) ===

                    where cppcheck >nul 2>nul
                    if %ERRORLEVEL% NEQ 0 (
                        echo cppcheck not installed - skipping analysis
                        exit /b 0
                    )

                    cppcheck --enable=all ^
                             --inconclusive ^
                             --xml --xml-version=2 ^
                             --force ^
                             -I include ^
                             src 2> %REPORTS%\\cppcheck.xml
                '''
            }
            post {
                always {
                    script {
                        if (fileExists("${REPORTS}/cppcheck.xml")) {
                            recordIssues tools: [
                                cppCheck(pattern: "${REPORTS}/cppcheck.xml")
                            ]
                        } else {
                            echo "cppcheck report not found - skipping quality report"
                        }
                    }
                }
            }
        }

        stage('Build All ECUs') {
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
                        tests\\test_*.c ^
                        src\\core\\*.c ^
                        unity\\unity.c

                    if exist %BUILD_DIR%\\test_core.exe (
                        %BUILD_DIR%\\test_core.exe > %REPORTS%\\unity_output.txt
                    ) else (
                        echo ERROR: test binary not built
                        exit /b 1
                    )

                    python scripts\\unity_to_junit.py ^
                        %REPORTS%\\unity_output.txt ^
                        %REPORTS%\\junit.xml
                '''
            }
            post {
                always {
                    junit allowEmptyResults: true,
                          testResults: "${REPORTS}/junit.xml"
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

                    lcov --capture --directory . --output-file %REPORTS%\\coverage.info || echo lcov failed
                    genhtml %REPORTS%\\coverage.info --output-directory %REPORTS%\\coverage || echo genhtml failed
                '''
            }
            post {
                always {
                    script {
                        if (fileExists("${REPORTS}/coverage/index.html")) {
                            publishHTML(target: [
                                reportDir: "${REPORTS}/coverage",
                                reportFiles: "index.html",
                                reportName: "Coverage Report",
                                keepAll: true,
                                alwaysLinkToLastBuild: true,
                                allowMissing: true
                            ])
                        } else {
                            echo "Coverage report not generated"
                        }
                    }
                }
            }
        }

        stage('Policy Gate') {
            steps {
                script {
                    echo "=== Policy Gate ==="

                    if (!fileExists("${REPORTS}/cppcheck.xml")) {
                        echo "No cppcheck report -> skipping gate"
                    } else {
                        def issues = scanForIssues(
                            tool: cppCheck(pattern: "${REPORTS}/cppcheck.xml")
                        )

                        if (issues.totalErrors > 0) {
                            error """
❌ Policy Gate failed
cppcheck errors: ${issues.totalErrors}
"""
                        }

                        echo "✅ Policy Gate passed"
                    }
                }
            }
        }
    }

    post {
        always {
            archiveArtifacts artifacts: 'reports/**', allowEmptyArchive: true
            echo "Pipeline finished - artifacts archived"
        }

        failure {
            echo "❌ Pipeline failed - check logs above"
        }

        success {
            echo "✅ Pipeline succeeded"
        }
    }
}
