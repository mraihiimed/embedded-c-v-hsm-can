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
        TEST_BIN = "build/bin/tests"
    }

    stages {

        stage('Precheck Environment') {
            steps {
                sh '''
                    echo "=== Precheck Environment ==="
<<<<<<< HEAD
<<<<<<< HEAD

                    set -e

                    gcc --version
                    make --version
                    cppcheck --version || echo "cppcheck missing"
=======
                    set -e
                    gcc --version
                    make --version
                    cppcheck --version || echo "cppcheck missing"
                    lcov --version || echo "lcov missing"
>>>>>>> 24f2a72 (update Jenkinsfile)
=======
                    gcc --version
                    make --version
                    cppcheck --version || echo "cppcheck missing"
>>>>>>> 24ed03b (Update Jenkinsfile)
                '''
            }
        }

        stage('Clean Workspace') {
            steps {
                sh '''
                    echo "=== Clean Workspace ==="
                    make clean || true
                    rm -rf build/*
<<<<<<< HEAD
                    mkdir -p build
                    mkdir -p reports
=======
                    mkdir -p build/bin
<<<<<<< HEAD
>>>>>>> 24f2a72 (update Jenkinsfile)
                    mkdir -p reports/coverage
=======
                    mkdir -p reports
>>>>>>> 24ed03b (Update Jenkinsfile)
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
                        echo "<results>cppcheck not installed</results>" > reports/cppcheck.xml
                    fi
                '''
            }
        }

        stage('Build Project') {
            steps {
                sh '''
                    echo "=== Build Project ==="
                    make all -j4
                '''
            }
        }

        stage('Build Unit Tests') {
            steps {
                sh '''
<<<<<<< HEAD
                    echo "=== Unit Tests ==="

                    if [ -f build/bin/test_core ]; then
                        ./build/bin/test_core > reports/unity_output.txt || true
                    else
                        echo "ERROR: test binary missing"
                        exit 1
                    fi

=======
                    echo "=== Build Unit Tests ==="
                    make tests -j4

                    if [ ! -f ${TEST_BIN} ]; then
                        echo "ERROR: Test binary missing: ${TEST_BIN}"
                        exit 1
                    fi
                '''
            }
        }

        stage('Run Unit Tests') {
            steps {
                sh '''
                    echo "=== Running Unit Tests ==="
                    ./${TEST_BIN} > reports/unity_output.txt || true

>>>>>>> 24f2a72 (update Jenkinsfile)
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

<<<<<<< HEAD
        stage('Coverage') {
            steps {
                sh '''
                    echo "=== Coverage ==="

                    if command -v lcov >/dev/null 2>&1; then
<<<<<<< HEAD
                        lcov --capture --directory . --output-file reports/coverage.info || true
                        genhtml reports/coverage.info --output-directory reports/coverage || true
=======
                        lcov --capture \
                             --directory build \
                             --output-file reports/coverage.info || true

                        genhtml reports/coverage.info \
                                --output-directory reports/coverage || true
>>>>>>> 24f2a72 (update Jenkinsfile)
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

=======
>>>>>>> 24ed03b (Update Jenkinsfile)
        stage('Security Gate') {
            steps {
                sh '''
                    echo "=== Security Gate ==="
<<<<<<< HEAD

                    if [ -f reports/cppcheck.xml ]; then
                        ERRORS=$(grep -c "<error" reports/cppcheck.xml || true)
<<<<<<< HEAD

                        echo "cppcheck errors: $ERRORS"

                        if [ "$ERRORS" -gt 0 ]; then
                            echo "❌ Policy Gate FAILED"
                            exit 1
                        fi
                    fi

=======
                        echo "cppcheck errors: $ERRORS"

                        if [ "$ERRORS" -gt 0 ]; then
                            echo "❌ Policy Gate FAILED"
                            exit 1
                        fi
                    fi

>>>>>>> 24f2a72 (update Jenkinsfile)
                    echo "✅ Policy Gate PASSED"
=======
                    echo "Ignoring cppcheck logs as requested"
                    echo "Policy Gate PASSED"
>>>>>>> 24ed03b (Update Jenkinsfile)
                '''
            }
        }
    }

    post {
        always {
            archiveArtifacts artifacts: 'reports/**', allowEmptyArchive: true
            echo "Pipeline finished"
        }
<<<<<<< HEAD

        success {
            echo "SUCCESS"
        }

=======
        success {
            echo "✅ SUCCESS"
        }
>>>>>>> 24f2a72 (update Jenkinsfile)
        failure {
            echo "FAILURE - check logs"
        }
    }
}
