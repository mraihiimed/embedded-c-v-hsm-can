pipeline {
    agent { label 'ubuntu' }   // 🔥 THIS replaces "agent any"

    environment {
        REPORTS = "reports"
    }

    stages {

        stage('Prepare Workspace') {
            steps {
                sh '''
                    mkdir -p ${REPORTS}
                    mkdir -p ${REPORTS}/coverage
                    mkdir -p build
                '''
            }
        }

        stage('Static Analysis - cppcheck') {
            steps {
                sh '''
                    cppcheck --enable=all \
                             --inconclusive \
                             --xml --xml-version=2 \
                             --force \
                             -I include \
                             src 2> ${REPORTS}/cppcheck.xml
                '''
            }
            post {
                always {
                    recordIssues tools: [cppCheck(pattern: "${REPORTS}/cppcheck.xml")]
                }
            }
        }

        stage('Build All ECUs') {
            steps {
                sh '''
                    make clean
                    make -j$(nproc)
                '''
            }
        }

        stage('Unit Tests') {
            steps {
                sh '''
                    gcc -Wall -Wextra -fprofile-arcs -ftest-coverage \
                        -I include \
                        -o build/test_core \
                        tests/test_*.c \
                        src/core/*.c \
                        unity/unity.c

                    ./build/test_core > ${REPORTS}/unity_output.txt || true

                    python3 scripts/unity_to_junit.py \
                        ${REPORTS}/unity_output.txt \
                        ${REPORTS}/junit.xml
                '''
            }
            post {
                always {
                    junit "${REPORTS}/junit.xml"
                }
            }
        }

        stage('Coverage') {
            steps {
                sh '''
                    lcov --capture --directory . --output-file ${REPORTS}/coverage.info
                    genhtml ${REPORTS}/coverage.info --output-directory ${REPORTS}/coverage
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
                    def issues = scanForIssues tool: cppCheck(pattern: "${REPORTS}/cppcheck.xml")
                    if (issues.totalErrors > 0) {
                        error "Policy Gate failed: cppcheck found ${issues.totalErrors} errors"
                    }
                }
            }
        }
    }
}
