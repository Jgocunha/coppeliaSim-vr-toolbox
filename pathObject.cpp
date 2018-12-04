// Copyright (c) 2018, Boris Bogaerts
// All rights reserved.

// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions 
// are met:

// 1. Redistributions of source code must retain the above copyright 
// notice, this list of conditions and the following disclaimer.

// 2. Redistributions in binary form must reproduce the above copyright 
// notice, this list of conditions and the following disclaimer in the 
// documentation and/or other materials provided with the distribution.

// 3. Neither the name of the copyright holder nor the names of its 
// contributors may be used to endorse or promote products derived from 
// this software without specific prior written permission.

// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR 
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
// HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "pathObject.h"
#include <iostream>
#include <cstdlib>
#include <vtkLinearTransform.h>
#include <vtkIdList.h>
pathObject::pathObject(int ID)
{
	//return;
	clientID = ID;
	simxInt succes;
	simxUChar *str;
	simxInt signalLength;
		succes = simxGetStringSignal(clientID, (simxChar*)"path", &str, &signalLength, simx_opmode_streaming);
		if (succes <2) { // if signal exist than we will are in business, otherwise ignore
			cout << "Connected to VREP path" << endl;
			exist = true;
		}
}


pathObject::~pathObject()
{
}

vtkSmartPointer<vtkActor> pathObject::getActor() {
	update();
	cells->InsertNextCell(lineData);

	polyData->SetPoints(points);
	polyData->SetLines(cells);

	lineMapper->SetInputData(polyData);
	lineActor->SetMapper(lineMapper);
	lineActor->GetProperty()->SetColor(0.7, 0.14, 0.56);
	lineActor->GetProperty()->SetLineWidth(5.0);
	lineActor->PickableOff();
	pose->Identity();
	pose->RotateX(-90);
	lineActor->SetUserTransform(pose);
	return lineActor;
}

void pathObject::update() {
	if (exist) {
		simxInt succes;
		simxUChar *str;
		simxInt signalLength;
		succes = simxGetStringSignal(clientID, (simxChar*)"path", &str, &signalLength, simx_opmode_buffer);
		//cout << "Path status : " << succes << endl;
		
		if (succes == simx_return_ok) {
			signalLength = (int)(signalLength / 4);
			points->SetNumberOfPoints((int)(signalLength / 3));
			memcpy(points->GetVoidPointer(0), (float*)str, signalLength * sizeof(float));
			//cells->SetNumberOfCells((int)((signalLength / 3)));
			//cells->Initialize();
			for (int i = cells->GetNumberOfCells()+1; i < (int)((signalLength / 3)); i++) {
					cells->InsertNextCell(2);
					cells->InsertCellPoint(i-1);
					cells->InsertCellPoint(i);
			}
		
		
			points->Modified();
			cells->Modified();
			polyData->Modified();
		}
		return;
	}
}

//for (int i = 0; i < points->GetNumberOfPoints(); i++) {
//	cout << points->GetPoint(i)[0] << "     " << points->GetPoint(i)[1] << "     " << points->GetPoint(i)[2] << endl;
//}

//for (int i = 0; i < cells->GetNumberOfCells(); i++) {
//	vtkSmartPointer<vtkIdList> ids = vtkSmartPointer<vtkIdList>::New();
//	cells->GetCell(i, ids);
//	cout << ids->GetNumberOfIds() << endl;
//	cout << ids->GetId(0) << "    " << ids->GetId(1) << endl;
//}