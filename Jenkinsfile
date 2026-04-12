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
                    echo Preparing workspace...

                    if not exist %REPORTS% mkdir %REPORTS%
                    if not exist %REPORTS%\\coverage mkdir %REPORTS%\\coverage
                    if not exist %BUILD_DIR% mkdir %BUILD_DIR%
                '''
            }
        }

        stage('Check Tools') {
            steps {
                bat '''
                    echo Checking required tools...

                    where gcc     || echo gcc NOT FOUND
                    where make    || echo make NOT FOUND
                    where cppcheck || echo cppcheck NOT FOUND
                    where python  || echo python NOT FOUND
                '''
            }
        }

        stage('Static Analysis - cppcheck') {
            steps {
                bat '''
                    echo Running static analysis...

                    make cppcheck REPORTS=%REPORTS%
                '''
            }
            post {
                always {
                    recordIssues tools: [
                        cppCheck(pattern: "${REPORTS}/cppcheck.xml")
                    ]
                }
            }
        }

        stage('Build All ECUs') {
            steps {
                bat '''
                    echo Building project...

                    make clean
                    make all -j4
                '''
            }
        }

        stage('Unit Tests') {
            steps {
                bat '''
                    echo Running unit tests...

                    gcc -Wall -Wextra -fprofile-arcs -ftest-coverage ^
                        -I include ^
                        -o %BUILD_DIR%\\test_core ^
                        tests\\test_*.c ^
                        src\\core\\*.c ^
                        unity\\unity.c

                    %BUILD_DIR%\\test_core.exe > %REPORTS%\\unity_output.txt

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
                    echo Running coverage analysis (optional)...

                    lcov --capture --directory . --output-file %REPORTS%\\coverage.info || echo lcov failed
                    genhtml %REPORTS%\\coverage.info --output-directory %REPORTS%\\coverage || echo genhtml failed
                '''
            }
            post {
                always {
                    publishHTML(target: [
                        reportDir: "${REPORTS}/coverage",
                        reportFiles: "index.html",
                        reportName: "Coverage Report",
                        keepAll: true,
                        alwaysLinkToLastBuild: true,
                        allowMissing: true
                    ])
                }
            }
        }

        stage('Policy Gate') {
            steps {
                script {
                    echo "Running quality gate..."

                    def issues = scanForIssues(
                        tool: cppCheck(pattern: "${REPORTS}/cppcheck.xml")
                    )

                    if (issues.totalErrors > 0) {
                        error """
                        ❌ Policy Gate failed:
                        cppcheck found ${issues.totalErrors} errors
                        """
                    }

                    echo "✅ Policy Gate passed"
                }
            }
        }
    }

    post {
        always {
            archiveArtifacts artifacts: 'reports/**', allowEmptyArchive: true
            echo "Pipeline finished. Reports archived."
        }
    }
}
