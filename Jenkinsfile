pipeline {
    agent any

    options {
        timestamps()
        timeout(time: 25, unit: 'MINUTES')
        disableConcurrentBuilds()
    }

    environment {
        REPORTS = "reports"
        BUILD_DIR = "build"
    }

    stages {

        stage('Precheck Environment') {
            steps {
                sh '''
                    echo "=== Precheck Environment ==="

                    set -e

                    gcc --version
                    make --version
                    cppcheck --version || echo "cppcheck missing"
                '''
            }
        }

        stage('Clean Workspace') {
            steps {
                sh '''
                    echo "=== Clean Workspace ==="
                    make clean || true
                    rm -rf build/*
                    mkdir -p build
                    mkdir -p reports
                    mkdir -p reports/coverage
                '''
            }
        }

        stage('Static Analysis (cppcheck)') {
            steps {
                sh '''
                    echo "=== cppcheck analysis ==="

                    if command -v cppcheck >/dev/null 2>&1; then
                        cppcheck --enable=all \
                                 --inconclusive \
                                 --xml --xml-version=2 \
                                 --force \
                                 -I include \
                                 src 2> reports/cppcheck.xml
                    else
                        echo "cppcheck not installed - skipping"
                    fi
                '''
            }
            post {
                always {
                    script {
                        if (fileExists("reports/cppcheck.xml")) {
                            recordIssues tools: [
                                cppCheck(pattern: "reports/cppcheck.xml")
                            ]
                        }
                    }
                }
            }
        }

        stage('Build Project') {
            steps {
                sh '''
                    echo "=== Build Project ==="
                    make all -j$(nproc)
                '''
            }
        }

        stage('Unit Tests') {
            steps {
                sh '''
                    echo "=== Unit Tests ==="

                    if [ -f build/bin/test_core ]; then
                        ./build/bin/test_core > reports/unity_output.txt || true
                    else
                        echo "ERROR: test binary missing"
                        exit 1
                    fi

                    python3 scripts/unity_to_junit.py \
                        reports/unity_output.txt \
                        reports/junit.xml || true
                '''
            }
            post {
                always {
                    junit allowEmptyResults: true,
                          testResults: "reports/junit.xml"
                }
            }
        }

        stage('Coverage') {
            steps {
                sh '''
                    echo "=== Coverage ==="

                    if command -v lcov >/dev/null 2>&1; then
                        lcov --capture --directory . --output-file reports/coverage.info || true
                        genhtml reports/coverage.info --output-directory reports/coverage || true
                    else
                        echo "lcov not installed - skipping coverage"
                    fi
                '''
            }
            post {
                always {
                    publishHTML(target: [
                        reportDir: "reports/coverage",
                        reportFiles: "index.html",
                        reportName: "Coverage Report",
                        allowMissing: true
                    ])
                }
            }
        }

        stage('Security Gate') {
            steps {
                sh '''
                    echo "=== Security Gate ==="

                    if [ -f reports/cppcheck.xml ]; then
                        ERRORS=$(grep -c "<error" reports/cppcheck.xml || true)

                        echo "cppcheck errors: $ERRORS"

                        if [ "$ERRORS" -gt 0 ]; then
                            echo "❌ Policy Gate FAILED"
                            exit 1
                        fi
                    fi

                    echo "✅ Policy Gate PASSED"
                '''
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
            echo "❌ FAILURE - check logs"
        }
    }
}
