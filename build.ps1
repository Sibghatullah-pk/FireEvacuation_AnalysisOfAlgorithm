# PowerShell build script for Fire Evacuation Simulator
# Run with: powershell -ExecutionPolicy Bypass -File build.ps1

Write-Host ""
Write-Host "========================================"
Write-Host "Fire Evacuation Simulator - Full Build"
Write-Host "========================================"
Write-Host ""

# Check if g++ is available
try {
    g++ --version | Out-Null
} catch {
    Write-Host "[ERROR] g++ not found. Please install MinGW or add it to PATH." -ForegroundColor Red
    exit 1
}

# Check if Node.js is available
try {
    node --version | Out-Null
} catch {
    Write-Host "[ERROR] Node.js not found. Please install from https://nodejs.org/" -ForegroundColor Red
    exit 1
}

Write-Host "[1/5] Building C++ Simulator..." -ForegroundColor Cyan
$compileCmd = @(
    'g++',
    '-std=c++17',
    '-Iinclude',
    'src/main.cpp',
    'src/SimulationManager.cpp',
    'src/Grid.cpp',
    'src/Fire.cpp',
    'src/Person.cpp',
    'src/PathfindingAlgorithms.cpp',
    'src/StateOutput.cpp',
    '-o',
    'fire_sim.exe'
)

& $compileCmd
if ($LASTEXITCODE -ne 0) {
    Write-Host "[ERROR] C++ compilation failed." -ForegroundColor Red
    exit 1
}
Write-Host "[✓] C++ build successful: fire_sim.exe" -ForegroundColor Green

Write-Host ""
Write-Host "[2/5] Installing backend dependencies..." -ForegroundColor Cyan
Push-Location backend
npm install
if ($LASTEXITCODE -ne 0) {
    Write-Host "[ERROR] Backend npm install failed." -ForegroundColor Red
    Pop-Location
    exit 1
}
Pop-Location
Write-Host "[✓] Backend dependencies installed" -ForegroundColor Green

Write-Host ""
Write-Host "[3/5] Installing frontend dependencies..." -ForegroundColor Cyan
Push-Location frontend
npm install
if ($LASTEXITCODE -ne 0) {
    Write-Host "[ERROR] Frontend npm install failed." -ForegroundColor Red
    Pop-Location
    exit 1
}
Pop-Location
Write-Host "[✓] Frontend dependencies installed" -ForegroundColor Green

Write-Host ""
Write-Host "[4/5] Building frontend (React + Vite)..." -ForegroundColor Cyan
Push-Location frontend
npm run build
if ($LASTEXITCODE -ne 0) {
    Write-Host "[ERROR] Frontend build failed." -ForegroundColor Red
    Pop-Location
    exit 1
}
Pop-Location
Write-Host "[✓] Frontend build successful" -ForegroundColor Green

Write-Host ""
Write-Host "========================================"
Write-Host "[✓] Build Complete!" -ForegroundColor Green
Write-Host "========================================"
Write-Host ""
Write-Host "Next steps:" -ForegroundColor Yellow
Write-Host ""
Write-Host "1. Start the backend server:"
Write-Host "   cd backend && npm start"
Write-Host ""
Write-Host "2. Open your browser to:"
Write-Host "   http://localhost:5000"
Write-Host ""
Write-Host "For development with hot reload:"
Write-Host "   cd frontend && npm run dev"
Write-Host ""
