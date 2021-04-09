import { Component } from "@angular/core";
import { Map } from "@app/class/map";
import { Vec3 } from "@app/class/vec3";
import { MapCatalogService } from "@app/service/map-catalog/map-catalog.service";
import { SocketService } from "@app/service/socket.service";
 


@Component({
  selector: "app-map-list",
  templateUrl: "./map-list.component.html",
  styleUrls: ["./map-list.component.scss"],
})
export class MapListComponent { 
  points : Vec3[] = []
  constructor(public mapCatalogService : MapCatalogService, public socketService: SocketService){
  }
  public get maps(): Map[] {
    // return this.mapCatalogService.map_list;
    return [];
  }

  getMapSelected(id:Number):void {
    this.socketService.getSelectedMap(id);
  }

}